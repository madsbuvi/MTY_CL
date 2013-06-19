/***********************************************************************
 *
 *	file: wdict.c
 *
 *	辞書登録・判定など
 *
 *	$Id: wdict.c 190 2008-01-15 02:54:13Z chapuni $
 *
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "debug.h"
#include "synth.h"
#include "util.h"
#include "wdict.h"

#define HASH_NCHARS	4
#define HASH_SEED	16381
#define HASH_LEAP	37
#define BMHASH_SIZ	((HASH_SEED + ALU_BITS - 1) / ALU_BITS)

/***************************************************************
 *
 *	辞書構造
 *
 */

struct WDK
{
  uint32_t k;	/* キー */
  uint32_t i;	/* 範囲終点 */
};

struct WDW
{
  uint64_t w;	/* 語 */
  uint16_t xp;	/* 区別ナシ */
};

struct WDICT
{
  /* Hash bitmap (for truncation)
	 Empirically, (something)ケツで判定するのがよろし */
  ALU_T bmhash[BMHASH_SIZ];

  char pos;
  char len;

  /* Primary search key */
  int nwdk;
  struct WDK *wdk;

  /* Words */
  struct WDW *words;

  /* The following fields are not used when searching */

  /* Cross reference to corresponding node */
  struct ITREE *node;

  /* Total number of words */
  int nwords;

  /* Record covering (?) */
  uint64_t *cov;
};

#define N_CLS	3

static
struct WDICT *g_dict[N_CLS][11][11];

/* tn の分だけ並んでいる */
static int min_dictpool, n_dictpool;
static
struct WDICT g_dictpool[0x1000];

/***************************************************************
 *
 *	xpize() ... Normalize based on the bitmap xp
 *	normalize() ... Normalization
 *	hash() ... Subtle hash function (normalization required)
 *
 */

static
uint64_t
xpize(uint64_t m, unsigned xp, unsigned len)
{
  int i;

  for (i = 0; i < len; i++)
	if (xp & (1 << i))
	  {
		unsigned c = (m >> 6 * i) & 077;
		unsigned s;
		if (c >= 046)
		  s = 26;
		else if (c == 001)
		  s = 1;
		else
		  continue;
		m -= (uint64_t)s << 6 * i;
	  }

  return m;
}

static
uint64_t
normalize(uint64_t m)
{
  return xpize(m, -1, 10);
}

static
unsigned
hash(unsigned m)
{
  int i;
  unsigned a = 0;

  for (i = 0; i < HASH_NCHARS; i++, m >>= 6)
	{
	  unsigned c = m & 077;
#if DEBUG>=1
	  assert(c != 001);
	  assert(c < 046);
#endif
	  if (c >= 002)
		c--;
	  a = 37 * a + c;
	}

  return a % HASH_SEED;
}

/***************************************************************
 *
 *	Produce a cover of what symbols a word covers
 *
 */

static
void
wdict_mkcov(uint64_t *cov, int len, uint64_t w, unsigned xp)
{
  int i;
  for (i = 0; i < len; i++, w >>= 6, xp >>= 1)
	cov[i] |= (((xp & 1)		/* Case insensitive */
				? ((w & 077) < 2
				   ? 3ULL		/* [./] */
				   : (1ULL | (1ULL << 26)))
				: 1ULL)
			   << (w & 077));
}

/***************************************************************
 *
 *	Add a word to the dictionary
 *
 */

struct WDICT *
wdict_add(int tn,
		  int pos,
		  struct DICT_CHAR const *word)
{
  struct WDICT *pd;
  struct WDICT *rpd = NULL;
  int len		= word->len;
  unsigned attr;
  unsigned xp	= word->xp;
  uint64_t text	= word->bmp;

  /* Determine class */
  switch (word->attr)
	{
	case CA_U:	attr = 1; break;
	case CA_U | CA_C:
	case CA_C:	attr = 2; break;
	default:	attr = 0; break;
	}

  assert(attr < N_CLS);
  pd = g_dict[attr][pos][len];
  if (pd == NULL)
	{
	  assert(tn >= 0);
	  assert(tn < sizeof(g_dictpool) / sizeof(*g_dictpool));
	  g_dict[attr][pos][len] = pd = &g_dictpool[tn];
	  if (n_dictpool == 0)
		min_dictpool = tn;
	  else if (min_dictpool >= tn)
		min_dictpool = tn;
	  if (n_dictpool <= tn)
		n_dictpool = tn + 1;
	  pd->pos = pos;
	  pd->len = len;
	  pd->words = malloc(1 * sizeof(*pd->words));
	  assert(pd->words != NULL);
	  pd->cov = calloc(10, sizeof(*pd->cov));
	  rpd = pd;
	}

  /* Expand by powers of two*/
  if (pd->nwords >= 1
	  && IS_POWER2(pd->nwords))
	{
	  assert(pd->words != NULL);
	  pd->words = realloc(pd->words,
						  2 * pd->nwords * sizeof(*pd->words));
	  assert(pd->words != NULL);
	}

  /* Sign up a word in the dictionary */
  assert(text == xpize(text, xp, len));
  pd->words[pd->nwords].xp = xp;
  pd->words[pd->nwords++].w = text;

  /* Record what symbols this covers */
  wdict_mkcov(pd->cov, len, text, xp);

  return rpd;	/* Only returns anything if it's a new entry */
}

/***************************************************************
 *
 *	Associate a node with a dictionary
 *
 */

void
wdict_setnode(struct WDICT *pd,
			  struct ITREE *node)
{
  assert(pd != NULL);
  assert(node != NULL);
  pd->node = node;
}

/***************************************************************
 *
 *	辞書の被覆を固定する
 *	ついでにソートもしておく(まだてぬき)
 *
 */

static
int
cmp_dict(void const *pa, void const *pb)
{
  struct WDW const *a = pa;
  struct WDW const *b = pb;
  uint64_t i = normalize(a->w);
  uint64_t j = normalize(b->w);
  if (i < j) return -1;
  if (i > j) return 1;
  if (a->w < b->w) return -1;
  if (a->w > b->w) return 1;
  return (int)(a->xp - (int)b->xp);
}

static
void
wdict_setup_words(struct WDICT *pd)
{
  int i, j, k;

  /* First, sort */
  qsort(pd->words,
		pd->nwords, sizeof(*pd->words),
		cmp_dict);

  /* Assemble the primary search */
  k = 0;
  for (i = j = 0; i < pd->nwords; i++)
	{
	  uint32_t w = normalize(pd->words[i].w) >> 6 * (pd->len - HASH_NCHARS);
	  if (i == 0)
		{
		  /* The first entry */
		  pd->wdk = malloc(sizeof(*pd->wdk));
		  assert(pd->wdk != NULL);
		  pd->wdk[k].k = w;
		}
	  else if (pd->wdk[k].k != w)
		{
		  pd->wdk[k++].i = i;
		  if (IS_POWER2(k))
			pd->wdk = realloc(pd->wdk,
							  2 * k * sizeof(*pd->wdk));
		  assert(pd->wdk != NULL);
		  pd->wdk[k].k = w;
		}

	  /* Record the zero-order hash */
	  w = hash(w);
	  pd->bmhash[w / ALU_BITS] |= (ALU_T)1 << (w & (ALU_BITS - 1));
	}
  pd->wdk[k].i = i;
  pd->nwdk = k + 1;
}

void
wdict_setup_lr(void)
{
  int i;
  int cls, pos, len;

  for (cls = 0; cls < N_CLS; cls++)
	for (pos = 1; pos < 11; pos++)
	  for (len = 1; len <= 10; len++)
		{
		  struct WDICT *pd = g_dict[cls][pos][len];

		  if (pd == NULL)
			continue;

		  assert(pd->nwords > 0);
		  assert(pd->node != NULL);
		  assert(pd->node->dict == pd);
		  assert(pd->node->ref > 0);
		  assert(pd->pos == pos);
		  assert(pd->len == len);

		  for (i = 0; i < len; i++)
			synth_add(pd->node,
					  synth_make_lr(pd->cov[i], pos + i),
					  NULL);
		  wdict_setup_words(pd);

#if DEBUG>=1
		  fprintf(stderr,
				  "dic(%d/%2d/%2d):%08X%08X/%08X%08X(%8d)\n",
				  cls, pos, len,
				  (unsigned)(pd->words[0].w >> 32), (unsigned)pd->words[0].w,
				  (unsigned)(pd->words[pd->nwords - 1].w >> 32), (unsigned)pd->words[pd->nwords - 1].w,
				  pd->nwords);
#endif
		}
#if DEBUG>=1
  fprintf(stderr,
		  "wdict=%d(%d..%d)\n",
		  n_dictpool - min_dictpool, min_dictpool,  n_dictpool - 1);
#endif
}

/***************************************************************
 *
 *	Look for hits
 *	てぬきバイナリサーチでどうにかなってしまうあたりがすごい。
 *  ^~= It's amazing that this somehow becomes a shoddy binary search!
 *
 */

int
wdict_ishit(SLICE const *hit,	/* 生成比較器がセットしたヒット配列[tn] */
			int an, int ab,		/* SLICE::a[an].bit(ab) */
			uint64_t bmp)		/* デコード済み文字列 */
{

  int i;
  uint64_t bi;

  bmp &= (1ULL << 6 * 10) - 1;
  bi = normalize(bmp);
  for (i = min_dictpool; i < n_dictpool; i++)
	{
	  struct WDICT const *pd = &g_dictpool[i];
	  int len = pd->len;
	  if (len > 0
		  && (hit[i].a[an] & ((ALU_T)1 << ab)))
		{
		  uint64_t m;
		  unsigned k = ((bi >> 6 * ((pd->pos - 1) + len - HASH_NCHARS))
						& ((1 << 6 * HASH_NCHARS) - 1));
		  unsigned ia, ib;
		  unsigned i, j;

		  /* 零次ヒット判定 */
		  i = hash(k);
		  if (!(pd->bmhash[i / ALU_BITS] & ((ALU_T)1 << (i & (ALU_BITS - 1)))))
			continue;

		  /* 一次キーを引く */
		  for (ia = 0, ib = pd->nwdk;
			   ib > 0;
			   ib >>= 1)
			{
			  uint32_t x, y;
			  i = ia + (ib >> 1);
			  x = k;
			  y = pd->wdk[i].k;
#if DEBUG>=2
			  printf("%08o/%08o %5d(%5d)%5d %5d\n",
					 x, y, ia, i, ib, pd->wdk[i].i);
#endif
			  if (x == y)
				goto found_1st;
			  else if (x > y)
				{
				  ia = i + 1;
				  ib--;
				}
			}
		  continue;	/* not found */

		  /* 一次にヒットしたのでのんびりと二次範囲を決める */
		found_1st:
		  m = (bi >> 6 * (pd->pos - 1)) & ((1ULL << 6 * pd->len) - 1);
#if DEBUG>=2
		  printf("hit:k=%08o i=%d\n", k, i);
		  printf("%010o%010o (%d,%d)/%d\n",
				 (unsigned)(m >> 30),
				 (unsigned)(m & 07777777777),
				 (i == 0 ? 0 : pd->wdk[i - 1].i),
				 pd->wdk[i].i,
				 pd->nwords);
#endif
		  for (ia = (i == 0 ? 0 : pd->wdk[i - 1].i),
				 ib = pd->wdk[i].i - ia;
			   ib > 0;
			   ib >>= 1)
			{
			  uint64_t x, y;
			  i = ia + (ib >> 1);
			  x = m;
			  y = normalize(pd->words[i].w);
#if DEBUG>=2
			  printf("%010o%010o %5d(%5d)%5d %5d\n",
					 (unsigned)(y >> 30),
					 (unsigned)(y & 07777777777),
					 ia, i, ib, pd->nwords);
#endif
			  if (x == y)
				{
				  /* 近辺をリニアサーチ(乱暴だね!) */
				  x = ((bmp >> 6 * (pd->pos - 1)) & ((1ULL << 6 * pd->len) - 1));
#if DEBUG>=2
				  printf("%010o%010o:X\n",
						 (unsigned)(x >> 30),
						 (unsigned)(x & 07777777777));
#endif
				  for (j = i;
					   j < pd->nwords && normalize(pd->words[j].w) == m;
					   j++)
					{
					  uint64_t xi = xpize(x, pd->words[j].xp, len);
#if DEBUG>=2
					  printf("%010o%010o:%03X\n%010o%010o:%d\n",
							 (unsigned)(xi >> 30),
							 (unsigned)(xi & 07777777777),
							 pd->words[j].xp,
							 (unsigned)(y >> 30),
							 (unsigned)(y & 07777777777),
							 j);
#endif
					  if (pd->words[j].w == xi)
						return 1;
					}

				  for (j = i - 1;
					   0 <= j && j < pd->nwords
						 && normalize(pd->words[j].w) == m;
					   j--)
					{
					  uint64_t xi = xpize(x, pd->words[j].xp, len);
#if DEBUG>=2
					  printf("%010o%010o:%03X\n%010o%010o:%d\n",
							 (unsigned)(xi >> 30),
							 (unsigned)(xi & 07777777777),
							 pd->words[j].xp,
							 (unsigned)(y >> 30),
							 (unsigned)(y & 07777777777),
							 j);
#endif
					  if (pd->words[j].w == xi)
						return 1;
					}

				  /* 次の DICT の判定へ。 */
				  break;
				}

			  if (x > y)
				{
				  ia = i + 1;
				  ib--;
				}
			}
		}
	}

  return 0;
}

/***************************************************************
 *
 *	辞書内容をダンプ; for debug
 *
 */

void
wdict_dump(int lv, struct WDICT const *dict)
{
  int cls, pos, len;
  char const *t = &"\t\t\t\t\t\t\t\t\t\t"[10 - lv];

  assert(dict != NULL);

  for (cls = 0; cls < N_CLS; cls++)
	for (pos = 0; pos <= 10; pos++)
	  for (len = 1; len <= 10; len++)
		if (dict == g_dict[cls][pos][len])
		  printf("%sdic=%d/%d/%d words=%d\n",
				 t,
				 cls, pos, len,
				 dict->nwords);
#if DEBUG>=2
  for (i = 0; i < dict->nwords; i++)
	printf("%s\t%010o%010o:%4d xp=%03X\n",
		   t,
		   (unsigned)(dict->words[i].w >> 30),
		   (unsigned)(dict->words[i].w & 07777777777),
		   i,
		   dict->words[i].xp);
#endif
}

/***************************************************************
 *
 *	Create copy of dictionary structures compatible with the GPU
 *
 */
 
//OpenCL versions of the wdict structure

struct WDICT_cl
{
	uint32_t bmhash[BMHASH_SIZ];

	int8_t pos;
	int8_t len;

	int32_t nwdk;
	int32_t wdk;

	int32_t words;
	int32_t nwords;
};

extern void wdict_setup_gpu(void **dictpool, void **wdk_pool, void **wdw_pool,
							int32_t *dictpoolsize, int32_t *wdksize, int32_t *wdwsize,
							int32_t *ret_min_dictpool, int32_t *ret_n_dictpool){
	*ret_min_dictpool = min_dictpool;
	*ret_n_dictpool = n_dictpool;
	
	*dictpoolsize = sizeof(struct WDICT_cl)*n_dictpool;
	struct WDICT_cl *cldicts = (struct WDICT_cl*)calloc(*dictpoolsize,1);
	*dictpool = cldicts;
	int i = 0, j = 0;
	int nwdk = 0;
	int nwords = 0;
	
	for(; i < n_dictpool; i++){
		struct WDICT_cl *cldict = &cldicts[i];
		struct WDICT *hdict = &g_dictpool[i];
		memcpy(&cldict->bmhash[0],&hdict->bmhash[0],sizeof(cldict->bmhash));
		cldict->wdk = nwdk;
		nwdk += cldict->nwdk = hdict->nwdk;
		cldict->words = nwords;
		nwords += cldict->nwords = hdict->nwords;
		cldict->pos = hdict->pos;
		cldict->len = hdict->len;
	}
	
	printf("Further work...\n");
	
	*wdksize = nwdk*sizeof(struct WDK);
	*wdwsize = nwords*sizeof(struct WDW);
	struct WDK *clwdk = (struct WDK *)calloc(*wdksize,1);
	struct WDW *clwdw = (struct WDW *)calloc(*wdwsize,1);
	*wdk_pool = clwdk;
	*wdw_pool = clwdw;
	for(i = 0; i < n_dictpool; i++){
		struct WDICT *hdict = &g_dictpool[i];
		struct WDICT_cl *cldict = &cldicts[i];
		memcpy(&clwdk[cldict->wdk],hdict->wdk,cldict->nwdk*sizeof(struct WDK));
		memcpy(&clwdw[cldict->words],hdict->words,cldict->nwords*sizeof(struct WDW));
	}
	
	printf("Ensuring correctness\n");
	
	for(i=0; i < n_dictpool; i++){
			struct WDICT *hdict = &g_dictpool[i];
		struct WDICT_cl *cldict = &cldicts[i];
		int truth = 1;
		for(j=0; j < BMHASH_SIZ; j++)assert(hdict->bmhash[j]==cldict->bmhash[j]);
		assert(truth);
		assert(hdict->pos==cldict->pos);
		assert(hdict->len==cldict->len);
		assert(hdict->nwdk==cldict->nwdk);
		assert(hdict->nwords==cldict->nwords);
		for(j=0; j < cldict->nwdk;j++)assert(hdict->wdk[j].k==clwdk[cldict->wdk+j].k);
		for(j=0; j < cldict->nwdk;j++)assert(hdict->wdk[j].i==clwdk[cldict->wdk+j].i);
		for(j=0; j < cldict->nwords;j++)assert(hdict->words[j].w==clwdw[cldict->words+j].w);
		for(j=0; j < cldict->nwords;j++)assert(hdict->words[j].xp==clwdw[cldict->words+j].xp);
	}

}

/*
 *	Local Variables:
 *		tab-width:	4
 *	End:
 *
 * EOF */
