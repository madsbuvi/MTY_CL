/***********************************************************************
 *
 *	file: mty.c
 *
 *	Main file of mty cl
 *
 *	Original source written by Chapuni (http://naniya.sourceforge.jp/, chapuni@users.sourceforge.jp)
 *	Modified by Mads Buvik Sandvei (https://github.com/madsbuvi/, madsbuvi@stud.ntnu.no)
 *
 */

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>
#include <pthread.h>

#if defined(WIN32)

#include <windows.h>
#include <process.h>

#elif defined(__GNUC__)

#include <sys/time.h>

#endif

#include "desconst.h"
#include "expr_parse.h"
#include "hit.h"
#include "key.h"
#include "log.h"
#include "scoreboard.h"
#include "synth.h"
#include "util.h"
#include "mty_cl.h"

static HANDLE mutex_key;

/* CRYPT64 記述子 */
static
struct CRYPT64_DESC const *const crypt64_descs[] =
{
  &crypt64_desc,
};

/* 速度評価用 */
static int n_cpus;
static int n_gpus = 0;
static int dead_gpus = 0;
static uint64_t loop_cpu[1024];
static uint64_t loop_gpu[1024];

int
register_gpu(){
	WaitForSingleObject(mutex_key, INFINITE);
	int n = n_gpus;
	loop_gpu[n_gpus++] = 0;
	ReleaseMutex(mutex_key);
	return n;
}

void
unregister_gpu(){
	WaitForSingleObject(mutex_key, INFINITE);
	dead_gpus++;
	if(n_gpus == dead_gpus)set_start_time();
	ReleaseMutex(mutex_key);
}

void
register_trips_generated(int gpu, uint64_t n){
	loop_gpu[gpu] += n;
}

#define USEC_SEC 1000	/* 1秒 */


uint64_t
usec(void)
{
  uint32_t sec, msec;

#if !defined(WIN32)
  struct timeval tv;
  gettimeofday(&tv, NULL);
  sec = tv.tv_sec;
  msec = tv.tv_usec / (1000000 / USEC_SEC);
#else
  struct timeb tm;
  ftime(&tm);
  sec = tm.time;
  msec = tm.millitm / (1000 / USEC_SEC);
#endif

  return (uint64_t)USEC_SEC * sec + msec;
}

/***************************************************************
 *
 *	CPU capabilities を取得
 *	[XXX] あまりにも古いプロセッサのことは考えない。
 *
 *	a[4] = {EAX,EBX,ECX,EDX}
 *
 */

#if defined(__GNUC__)

#define cpuid(n,a,b,c,d)	\
	asm("cpuid"		\
		: "=a"(a), "=b"(b), "=c"(c), "=d"(d)	\
		: "a"(n))

#elif defined(WIN32)

#define cpuid(n,a,b,c,d)	\
	do {int r[4]; __cpuid(r,n);	\
	(a) = r[0]; (b) = r[1]; (c) = r[2]; (d) = r[3];} while (0)

#endif

static
unsigned
cpuid_getfflags(void)
{
  unsigned a, b, c, d;
  cpuid(1, a, b, c, d);
  return d;
}

static
int
cpuid_issupported(void)
{
  unsigned m = REQUIRED_CAPS;
  return !((cpuid_getfflags() ^ m) & m);
}

/***************************************************************
 *
 *	Packet for batch processing
 *
 */

static
struct PACKET_CRYPT64 *
packet_create(int n,	/* Packet count */
			  int tn,	/* Required number of work for the end element */
			  uint8_t const *ini_key)
{
  int i;
  int siz;
  void *p;
  intptr_t a = 128;
  struct PACKET_CRYPT64 *pkts;
  assert(IS_POWER2(sizeof(struct PACKET_CRYPT64)));
  assert(n >= 1);

  siz = (a - 1
		 + (n - 1) * sizeof(struct PACKET_CRYPT64)
		 + offsetof(struct PACKET_CRYPT64, param64.hit[tn]));
  p = calloc(siz, 1);
  /* band together */
  pkts = (struct PACKET_CRYPT64 *)(((intptr_t)p
									+ a - 1)
								   & -a);
#if DEBUG>=1
  fprintf(stderr,
		  "packet(n=%d,tn=%d) %d allocated; %p aligned to %p\n",
		  n, tn,
		  siz, p, pkts);
#endif

  /* 内部の初期化
	 コピーして回るのは、厳密には
	 最終要素のケツを破ってしまうことになるので
	 どうせ速度も要求されないしベタコード */
  for (i = 0; i < n; i++)
	{
	  int j, k;

	  /* t[16] は、内部演算で使用する、all 1 が入っている */
	  memset(&pkts[i].param64.t[T_INV], -1, sizeof(SLICE));

	  /* 固定キーの生成 */
	  key_init_sk(&pkts[i].key64);

	  /* キースケジュールをここに押し込めておく
		 従来は crypt64.S 内で完結するように引いていた */
	  for (j = 0; j < 28; j++)
		for (k = 0; k < N_ALU; k++)
		  pkts[i].key64.ks[j].a[k] = sizeof(WS_T) * ks_ls[j];

	  /* 念のため、鍵をここで落ち着けておく(不要?) */
	  for (j = 0; j < 8; j++)
		key_set64(&pkts[i].key64, j, pkts[i].uk.key[j] = ini_key[j], 0, 0x7F);
	}

  return pkts;
}

/***************************************************************
 *
 *	thread
 *
 */

#if defined(__GNUC__)

typedef int32_t ATOMWORD_T;

#define LOCK_INC(p)	\
asm volatile ("lock incl %0"	\
			  : "=m"(*(p))	\
			  : /*nil*/		\
			  : "memory")

#define LOCK_DEC(p)	\
asm volatile ("lock decl %0"	\
			  : "=m"(*(p))	\
			  : /*nil*/		\
			  : "memory")

#define LOCK_CAS(pd,s,r)	\
({	ATOMWORD_T a;							\
	asm volatile ("lock cmpxchg %2,%1"		\
				  : "=a"(a)					\
				  : "m"(*(pd)), "r"(s), "0"(r)	\
				  : "memory");a;})

#define LOCK_CASP(pd,s,r)	\
({	void *a;								\
	asm volatile ("lock cmpxchg %2,%1"		\
				  : "=a"(a)					\
				  : "m"(*(pd)), "r"(s), "0"(r)	\
				  : "memory");a;})

#elif defined(WIN32)

typedef LONG ATOMWORD_T;

#define LOCK_INC(p) InterlockedIncrement((LONG *)(p))
#define LOCK_DEC(p) InterlockedDecrement((LONG *)(p))
#define LOCK_CAS(pd,s,r) InterlockedCompareExchange((LONG *)(pd), s, r)
#define LOCK_CASP(pd,s,r) InterlockedCompareExchangePointer((PVOID *)(pd), (PVOID)(s), (PVOID)r)

#else
#error "configuration not implemented"
#endif

#if defined(WIN32)

typedef DWORD THREAD_TIMEOUT_T;

#define THREAD_INFINITE INFINITE

typedef HANDLE THREAD_TH_T;

#define thread_sleep(n) Sleep(n)
#define thread_create(th, proc, arg) {(th) = (HANDLE)_beginthread(proc, 8192, arg);}
#define thread_get_tid()	GetCurrentThread()
#define thread_set_priority(tid,n)	SetThreadPriority(tid, n)
#define thread_set_affinity(tid,m)	SetThreadAffinityMask(tid, (DWORD_PTR)1 << (m))

#elif defined(_POSIX_SOURCE)

#include <pthread.h>
#include <unistd.h>

typedef int THREAD_TIMEOUT_T;

#define THREAD_INFINITE	INT_MAX

#if defined(THREAD_PRIORITY_BELOW_NOROMAL) || defined(THREAD_PRIORITY_IDLE)
#error "unsupported implementation"
#endif

#define THREAD_PRIORITY_NORMAL	14
#define THREAD_PRIORITY_BELOW_NORMAL	15
#define THREAD_PRIORITY_IDLE	16

typedef pthread_t THREAD_TH_T;

#define thread_sleep(n) (usleep(1000 * (n)) != EINVAL || sleep((n) / 1000))
#define thread_create(th, proc, arg) thread_create_p(&(th), proc, arg)

static
void
thread_create_p(pthread_t *th, NORETURN (*proc)(void *), void *param)
{
  pthread_create(th, NULL, (void *(*)(void *))proc, param);
}

#if defined(__linux__)

/* The default scheduling policy
    You can also raise the idle thread priority settings
    So uninteresting. */

#include <linux/unistd.h>
_syscall0(pid_t,gettid)

#define thread_get_tid() gettid()

static
int thread_set_affinity(pid_t tid, int i)
{
  cpu_set_t m;
  CPU_ZERO(&m);
  CPU_SET(i, &m);
  return sched_setaffinity(tid, sizeof(m), &m);
}

#else

/* POSIX means you can't intervene with the scheduling on a per-thread-basis */

#endif

#else
#error "configuration not supported"
#endif

struct THREAD_PARAM
{
  /* The following is common to all threads */
  CODE_T *code;
  off_t code_cmp;
  unsigned seed;

  /* The following is thread specific */
#ifdef thread_set_priority
  int pri;
#endif

	int ID;
};

static
uint64_t
thread_avail(void)
{
	
#if !USE_MT

  return 0x1U;

#elif defined(WIN32)	/* Win32 API */
  DWORD_PTR mask, mask_s;
  if (!GetProcessAffinityMask(GetCurrentProcess(),
							  &mask,
							  &mask_s)
	  || !mask
	  || !mask_s)
	return 0x1U;
#if DEBUG>=1
  fprintf(stderr,
		  "m=%08X s=%08X\n",
		 (unsigned)mask,
		 (unsigned)mask_s);
#endif
	/* Couldn't translate this stuff, say nothing.
  if (popcnt64(mask_s) == 1)
	;
  else if (mask == mask_s)
	fprintf(stderr,
			"");
  else{
	fprintf(stderr,
			"最高速力の%g倍の力でてきとうにがんばるよ。\n",
			(double)popcnt64(mask) / popcnt64(mask_s));
			
  }*/
  return mask;

#elif defined(__linux__)	/* sched.h 拡張 */

  int i;
  uint64_t m = 0;
  cpu_set_t am;
  if (sched_getaffinity(getpid(), sizeof(am), &am) < 0)
	return 0x1U;

  for (i = 0; i < 64 && i < CPU_SETSIZE; i++)
	if (CPU_ISSET(i, &am))
	  m |= 1ULL << i;

  return m;
#else

  /* XXX プロセッサ数を調べ上げてください */
  return 0x01U;

#endif
}

static
NORETURN
thread_crypt64_new(void *a_param)
{	
	struct THREAD_PARAM *param = a_param;
	CODE_T *code = param->code;
	CODE_T *cmp = code + param->code_cmp;
	struct KS_KEY key;
	struct PACKET_CRYPT64 *pkt = packet_create(16, 1024*16, key.key);
	uint64_t *ploop;
	THREAD_TH_T th = thread_get_tid();

	WaitForSingleObject(mutex_key, INFINITE);

	ploop = &loop_cpu[n_cpus++];

	srand(usec() ^ param->seed ^ (unsigned)th);
	key_init(&key);
	ReleaseMutex(mutex_key);

#ifdef thread_set_priority
	thread_set_priority(th, param->pri);
#endif
	for (;;)
	{
		do
		{
			int j;
			for (j = 0; j < 8; j++)
			{
				key_set64(&pkt->key64, j, key.key[j], key.key[j] ^ pkt->uk.key[j], 0);
				pkt->uk.key[j] = key.key[j];
			}
			CALL_CRYPT64(code,
						 &pkt->key64,
						 &pkt->param64);
			CALL_CMP64(cmp,
					   pkt->param64.hit,
					   pkt->param64.lr);
			check_hit(pkt, pkt->param64.hit);
			*ploop += N_ALU * ALU_BITS;
		}
		while (key_inc(&key, 6, 8) || key_inc(&key, KEY_SHUFFLE_POS, 8));

		WaitForSingleObject(mutex_key, INFINITE);
		key_reset(&key, 0);
		ReleaseMutex(mutex_key);
	}

	/* notreached */
}

static struct status {
	uint64_t startTime;	/* Start time in milliseconds */
	uint64_t lastTime;	/* Time of last display in milliseconds */
	uint64_t loop;		/* Total number of searches */
	uint64_t cpu;		/* Number of searches on CPU */
	uint64_t gpu;		/* Number of seraches on GPU */
	uint64_t lastloop;	/* Last display of loop (?) */
} status;

void
set_start_time(){
	WaitForSingleObject(mutex_key, INFINITE);
	if(!status.startTime){
		status.startTime = status.lastTime = usec();
		int i;
		for (i = 0; i < n_cpus; i++) loop_cpu[i] = 0;
	}
	
	ReleaseMutex(mutex_key);
}


static void
initialize_gpu_searcher(pthread_t *gpu_handler){
	int num_devices = gpu_init(usec());
	fprintf(stderr,"Number of GPUs detected: %d\n",num_devices);
	int i = 0;
	for(;i<num_devices; i++){
		pthread_create( &gpu_handler[i], NULL, gpu_main, NULL);
	}
	//pthread_join(*gpu_handler, NULL);
}

/***************************************************************
 *
 *	Main Method
 *
 */

int
main(int argc, char *argv[])
{
  int i;
  CODE_T *code = NULL;
  off_t code_cmp;
  FILE *sfp;	/* scoreboard */
  struct ITREE *root_expr;
  uint64_t proc_mask;
  struct THREAD_PARAM *threads = NULL;
  int nthreads;
  int cr;

  /* Key string */
  struct KS_KEY key;

#define UPDATE_INTERVAL 8	/* Interval between screen updates in seconds */
  status.startTime = status.lastTime = 0;
  uint64_t curTime;
/*
 I couldn't really translate this:
 Average speed (kernels/s) * UPDATE_INTERVAL must not exceed UINT32_MAX。
 UINT32_MAX = 4294967295, 平均速度 = 100Mtrips/s なら、
 4294967295 / (100 * 1000 * 1000) = 42.949 秒まで。（和良
 LOOP_FACTOR が平均速度より十分小さければ、ほぼ指定間隔になる。
 LOOP_FACTOR * UINT32_MAX + LOOP_FACOTR 個検索するとオーバーフローする。ｗ
 */

  if (!cpuid_issupported())
	{
	  fprintf(stderr, "Processor ID or architecture was not recognized.\n");
	  exit(1);
	}

  assert((1 << N_STRIDE) == N_ALU * ALU_BITS);

  mutex_key = CreateMutex(NULL, FALSE, NULL);

  /* Read target.txt */
  root_expr = expr_parse("target.txt");

  /* 
	 Generate code and deploy
	 Change what code to generate depending
	 on number of threads
  */
  sfp = scoreboard_open();
  fwrite(crypt64_descs[0]->pro, 1, crypt64_descs[0]->cmp_pro - crypt64_descs[0]->pro, sfp);	/* prologue & コアループ */
  proc_mask = thread_avail();

#if 0
  if (0&&proc_mask == 1U)
	{
	  /* single */
	  code_cmp = 0;
	}
  else
#endif
	{
	  /* multi */
	  fwrite(crypt64_descs[0]->ep, 1, crypt64_descs[0]->ep_end - crypt64_descs[0]->ep, sfp);	/* epilogue */

	  /* 比較器のみを生成(前半)
		 
		 */
	  code_cmp = ftell(sfp);
	  fseek(sfp, (-code_cmp) & 63, SEEK_CUR);
	  code_cmp = ftell(sfp);
	  fwrite(crypt64_descs[0]->pro, 1, crypt64_descs[0]->crypt - crypt64_descs[0]->pro, sfp);	/* prologue */
	}

  /* Generate a comparison unit */
  fwrite(crypt64_descs[0]->cmp_pro, 1, crypt64_descs[0]->cmp_ep - crypt64_descs[0]->cmp_pro, sfp);	/* Preparation comparator */
  synth_synthesize(sfp,root_expr);
  fwrite(crypt64_descs[0]->cmp_ep, 1, crypt64_descs[0]->ep_end - crypt64_descs[0]->cmp_ep, sfp);	/* epilogue */
  /* put code into memory */
  code = scoreboard_map(sfp);

  /* Initialize the key */
  WaitForSingleObject(mutex_key, INFINITE);
  srand(usec());
  key_init(&key);
  ReleaseMutex(mutex_key);
  set_salt(code, crypt64_descs[0], key.key);

  if (log_open("log.txt") != 0) return 1;

  WaitForSingleObject(mutex_key, INFINITE);

  /* Production uncle working (>Google translate) */
  nthreads = 0;
  if (code_cmp)
	{
	  THREAD_TH_T h;
	  int ots = -1;
	  threads = calloc(2 * popcnt64(proc_mask), sizeof(*threads));
	  for (i = 0; i < 64; i++)
		if (proc_mask & (1ULL << i))
		  {
			if (0&&ots < 0)
			  {
				/* Own scheduling
					Gil apps Koyu system is to be set lower (Kamo) */
#ifdef WIN32
				h = GetCurrentProcess();
				SetPriorityClass(h, BELOW_NORMAL_PRIORITY_CLASS);
#endif
#if defined(thread_set_priority)
				/* I will fill the gap of heart (how gallant) */
				threads[nthreads].ID = nthreads;
				threads[nthreads].code = code;
				threads[nthreads].code_cmp = code_cmp;
				threads[nthreads].seed = rand();
				threads[nthreads].pri = THREAD_PRIORITY_IDLE;
				thread_create(h, thread_crypt64_new, &threads[nthreads]);
				nthreads++;
#endif
				if (!code_cmp)
				  break;

				/* do the rest of the settings later (?) */
				ots = i;
			  }
			else
			  {
				/* Another thread with slightly lower priority */
				threads[nthreads].code = code;
				threads[nthreads].code_cmp = code_cmp;
				threads[nthreads].seed = rand();
#ifdef thread_set_priority
				//threads[nthreads].pri = THREAD_PRIORITY_BELOW_NORMAL;
				threads[nthreads].pri = THREAD_PRIORITY_LOWEST;
#endif
				threads[nthreads].ID = nthreads;
				thread_create(h, thread_crypt64_new, &threads[nthreads]);
#ifdef thread_get_tid
				thread_set_affinity(h, i);
#endif
				nthreads++;
#if 1
				/* IDLE */
				threads[nthreads].code = code;
				threads[nthreads].code_cmp = code_cmp;
				threads[nthreads].seed = rand();
#ifdef thread_set_priority
				threads[nthreads].pri = THREAD_PRIORITY_IDLE;
#endif
				threads[nthreads].ID = nthreads;
				thread_create(h, thread_crypt64_new, &threads[nthreads]);
#ifdef thread_get_tid
				SetThreadAffinityMask(h, proc_mask);
#endif
				nthreads++;
#endif
			  }
		  }
#ifdef thread_get_tid
	  if (ots)
		thread_set_affinity(thread_get_tid(), ots);
#endif
	}
	
  //I'll take the libery of assuming this application won't have to handle more than 1024 gpus
  pthread_t gpu_handler[1024]; 
  initialize_gpu_searcher(gpu_handler);
  //return; 
  //fprintf(stderr,"Debug variables. %d\n",(int)&debug_variables);
  //fprintf(stderr, "Starting search!\n");
  ReleaseMutex(mutex_key);


  cr = 0;
  memset( &status, 0, sizeof( struct status ) );
  

  /* Search loop */
  for (;;)
	{
	  Sleep(5000);
	  /* Measure speed */
	  status.loop = 0;
	  status.cpu = 0;
	  status.gpu = 0;
	  for (i = 0; i < n_cpus; i++) status.cpu += loop_cpu[i];
	  for (i = 0; i < n_gpus; i++) status.gpu += loop_gpu[i];
	  status.loop = status.cpu + status.gpu;
		  
	  uint64_t diffTime;
	  int total, cpu, gpu;
	  
	  curTime = usec();
	  diffTime = curTime - status.startTime;
	  
	  /* Total (ktrips/sec) */
	  total = status.loop / ((1000 / USEC_SEC) * diffTime);
	  
	  /* Cpu (ktrips/sec) */
	  cpu = status.cpu / ((1000 / USEC_SEC) * diffTime);
	  
	  /* Gpu (ktrips/sec) */
	  gpu = status.gpu / ((1000 / USEC_SEC) * diffTime);
	  
	  status.lastTime = curTime;
	  status.lastloop = status.loop;
	  

	  fprintf(stderr,
			  "%3d.%03dMtrips/s [%3d.%03dMtrips/s @ CPU,%4d.%03dMtrip/s @ GPU] %I64u.%03I64uB total\r",
			  total / 1000, total % 1000,
			  cpu / 1000, cpu % 1000,
			  gpu / 1000, gpu % 1000,
			  status.loop/1000000000, (status.loop/1000000)%1000);
	  cr++;
	}

  return 0;
}

/*
 *	Local Variables:
 *		tab-width:	4
 *	End:
 *
 * EOF */
