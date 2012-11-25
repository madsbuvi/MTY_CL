/***********************************************************************
 *
 *	file: gpu.cl
 *
 *	Main file of the kernel side of the OpenCL implementation of mty cl
 *
 *	Written by Mads Buvik Sandvei
 *
 */

#define debugprints 0
#define asdasd 0

/*
 * printout defines for ease of debugging
 */
#define printi(a) if(index == 0)printf(#a ": %d\n",a);
#define printc(a) if(index == 0)printf(#a ": %c\n",a);
#define prints(s) if(index == 0)printf( s "\n");
#define printul(a) if(index == 0)printf(#a ": %I64u\n",a);
#define printu(a) if(index == 0)printf(#a ": %u\n",a);
#define printl(a) if(index == 0)printf(#a ": %I64\n",a);


typedef uchar 	uint8_t;
typedef ushort 	uint16_t;
typedef uint 	uint32_t;
typedef ulong 	uint64_t;
typedef char	int8_t;
typedef short	int16_t;
typedef int		int32_t;
typedef long	int64_t;

/*
 * Define DES attributes so i can easily change them
 * later kinda stopped using it since 64 types are just slow
 * as hell and i'll never use them. Should either move on to
 * make it more general again, or remove completely...
 */
typedef uint32_t type;
#define BITS 8*sizeof(type)

#define lsize 128

/*
 * From wdict.c
 */
 
#define BMHASH_SIZ ((16381 + 31) / 32)
#define HASH_NCHARS 4
#define HASH_SEED	16381
#define ALU_BITS 32

struct WDK
{
  uint32_t k;
  uint32_t i;
};

struct WDW
{
  uint64_t w;
  uint16_t xp;
};

struct WDICT
{
  uint32_t bmhash[BMHASH_SIZ];

  int8_t pos;
  int8_t len;

  int32_t nwdk;
  int32_t wdk;
  
  int32_t words;
  int32_t nwords;
};

inline uint64_t
xpize(uint64_t m, uint32_t xp, uint32_t len)
{
	int i;

	for (i = 0; i < len; i++){
		if (xp & (1 << i))
		{
			uint32_t c = (m >> 6 * i) & 077;
			uint32_t s;
			if (c >= 046)
			  s = 26;
			else if (c == 001)
			  s = 1;
			else
			  continue;
			m -= (uint64_t)s << 6 * i;
		}
	}

	return m;
}

inline uint64_t
normalise(uint64_t m)
{
	return xpize(m, 0xffffffff, 10);
}

inline uint32_t
hash(unsigned m)
{
  int i;
  uint32_t a = 0;

  for (i = 0; i < HASH_NCHARS; i++, m >>= 6)
	{
	  uint32_t c = m & 077;
	  if (c >= 002)
		c--;
	  a = 37 * a + c;
	}

  return a % HASH_SEED;
}

//Key Permutation.
__constant int keyperm[0x300] = {
	12, 46, 33, 52, 48, 20, 34, 55, 5, 13, 18, 40,
	4, 32, 26, 27, 38, 54, 53, 6, 31, 25, 19, 41,
	15, 24, 28, 43, 30, 3, 35, 22, 2, 44, 14, 23,
	51, 16, 29, 49, 7, 17, 37, 8, 9, 50, 42, 21,
	5, 39, 26, 45, 41, 13, 27, 48, 53, 6, 11, 33,
	52, 25, 19, 20, 31, 47, 46, 54, 55, 18, 12,
	34, 8, 17, 21, 36, 23, 49, 28, 15, 24, 37, 7,
	16, 44, 9, 22, 42, 0, 10, 30, 1, 2, 43, 35,
	14, 46, 25, 12, 31, 27, 54, 13, 34, 39, 47,
	52, 19, 38, 11, 5, 6, 48, 33, 32, 40, 41, 4,
	53, 20, 51, 3, 7, 22, 9, 35, 14, 1, 10, 23,
	50, 2, 30, 24, 8, 28, 43, 49, 16, 44, 17, 29,
	21, 0, 32, 11, 53, 48, 13, 40, 54, 20, 25, 33,
	38, 5, 55, 52, 46, 47, 34, 19, 18, 26, 27, 45,
	39, 6, 37, 42, 50, 8, 24, 21, 0, 44, 49, 9,
	36, 17, 16, 10, 51, 14, 29, 35, 2, 30, 3, 15,
	7, 43, 18, 52, 39, 34, 54, 26, 40, 6, 11, 19,
	55, 46, 41, 38, 32, 33, 20, 5, 4, 12, 13, 31,
	25, 47, 23, 28, 36, 51, 10, 7, 43, 30, 35, 24,
	22, 3, 2, 49, 37, 0, 15, 21, 17, 16, 42, 1, 50,
	29, 4, 38, 25, 20, 40, 12, 26, 47, 52, 5, 41,
	32, 27, 55, 18, 19, 6, 46, 45, 53, 54, 48, 11,
	33, 9, 14, 22, 37, 49, 50, 29, 16, 21, 10, 8,
	42, 17, 35, 23, 43, 1, 7, 3, 2, 28, 44, 36, 15,
	45, 55, 11, 6, 26, 53, 12, 33, 38, 46, 27, 18,
	13, 41, 4, 5, 47, 32, 31, 39, 40, 34, 52, 19,
	24, 0, 8, 23, 35, 36, 15, 2, 7, 49, 51, 28, 3,
	21, 9, 29, 44, 50, 42, 17, 14, 30, 22, 1, 31,
	41, 52, 47, 12, 39, 53, 19, 55, 32, 13, 4, 54,
	27, 45, 46, 33, 18, 48, 25, 26, 20, 38, 5, 10,
	43, 51, 9, 21, 22, 1, 17, 50, 35, 37, 14, 42,
	7, 24, 15, 30, 36, 28, 3, 0, 16, 8, 44, 55, 34,
	45, 40, 5, 32, 46, 12, 48, 25, 6, 52, 47, 20,
	38, 39, 26, 11, 41, 18, 19, 13, 31, 53, 3, 36,
	44, 2, 14, 15, 51, 10, 43, 28, 30, 7, 35, 0,
	17, 8, 23, 29, 21, 49, 50, 9, 1, 37, 41, 20,
	31, 26, 46, 18, 32, 53, 34, 11, 47, 38, 33, 6,
	55, 25, 12, 52, 27, 4, 5, 54, 48, 39, 42, 22,
	30, 17, 0, 1, 37, 49, 29, 14, 16, 50, 21, 43,
	3, 51, 9, 15, 7, 35, 36, 24, 44, 23, 27, 6, 48,
	12, 32, 4, 18, 39, 20, 52, 33, 55, 19, 47, 41,
	11, 53, 38, 13, 45, 46, 40, 34, 25, 28, 8, 16,
	3, 43, 44, 23, 35, 15, 0, 2, 36, 7, 29, 42, 37,
	24, 1, 50, 21, 22, 10, 30, 9, 13, 47, 34, 53,
	18, 45, 4, 25, 6, 38, 19, 41, 5, 33, 27, 52,
	39, 55, 54, 31, 32, 26, 20, 11, 14, 51, 2, 42,
	29, 30, 9, 21, 1, 43, 17, 22, 50, 15, 28, 23,
	10, 44, 36, 7, 8, 49, 16, 24, 54, 33, 20, 39,
	4, 31, 45, 11, 47, 55, 5, 27, 46, 19, 13, 38,
	25, 41, 40, 48, 18, 12, 6, 52, 0, 37, 17, 28,
	15, 16, 24, 7, 44, 29, 3, 8, 36, 1, 14, 9, 49,
	30, 22, 50, 51, 35, 2, 10, 40, 19, 6, 25, 45,
	48, 31, 52, 33, 41, 46, 13, 32, 5, 54, 55, 11,
	27, 26, 34, 4, 53, 47, 38, 43, 23, 3, 14, 1, 2,
	10, 50, 30, 15, 42, 51, 22, 44, 0, 24, 35, 16,
	8, 36, 37, 21, 17, 49, 26, 5, 47, 11, 31, 34,
	48, 38, 19, 27, 32, 54, 18, 46, 40, 41, 52, 13,
	12, 20, 45, 39, 33, 55, 29, 9, 42, 0, 44, 17,
	49, 36, 16, 1, 28, 37, 8, 30, 43, 10, 21, 2,
	51, 22, 23, 7, 3, 35, 19, 53, 40, 4, 55, 27,
	41, 31, 12, 20, 25, 47, 11, 39, 33, 34, 45, 6,
	5, 13, 38, 32, 26, 48, 22, 2, 35, 50, 37, 10,
	42, 29, 9, 51, 21, 30, 1, 23, 36, 3, 14, 24,
	44, 15, 16, 0, 49, 28
};
#if debugprints

__constant int tr_fp[64 + 2] =
{
  39,  7, 47, 15, 55, 23, 63, 31,
  38,  6, 46, 14, 54, 22, 62, 30,
  37,  5, 45, 13, 53, 21, 61, 29,
  36,  4, 44, 12, 52, 20, 60, 28,
  35,  3, 43, 11, 51, 19, 59, 27,
  34,  2, 42, 10, 50, 18, 58, 26,
  33,  1, 41,  9, 49, 17, 57, 25,
  32,  0, 40,  8, 48, 16, 56, 24,
  64, 64,
};

__constant int tr_sub[64] = {
	'.','/','0','1','2','3','4','5',
	'6','7','8','9','A','B','C','D',
	'E','F','G','H','I','J','K','L',
	'M','N','O','P','Q','R','S','T',
	'U','V','W','X','Y','Z','a','b',
	'c','d','e','f','g','h','i','j',
	'k','l','m','n','o','p','q','r',
	's','t','u','v','w','x','y','z'
};
#endif
#if asdasd
__constant uint32_t testtrip[8] = {
	0x36, 0x2A, 0xB6, 0x35, 0xA2, 0x88, 0xE9, 0x24
};
#endif
/*
 * defines to save typing and make kernel code more readable
 * note the coalesced memory access
 */
#define key(x) keys[(x)*size+index]
#define lk(x)  lkeys[(x)*lsize]
#define gb(x) blocks[(x)*size+index]
#define lb(x) lblock[(x)*lsize + lindex]
#define rb(x) rblock[(x)*lsize]
#define b(x) pblock[x]
#define work_area(x) lmem[(x)*lsize+lindex]
#define gwork_area(x) gwa[(x)*size+index]

#define xor_D(k1,b1,b2,b3,b4,b5,b6)\
a1 = k(k1+0)^b(b1);\
a2 = k(k1+1)^b(b2);\
a3 = k(k1+2)^b(b3);\
a4 = k(k1+3)^b(b4);\
a5 = k(k1+4)^b(b5);\
a6 = k(k1+5)^b(b6);

#define xor_E(k1,b1,b2,b3,b4,b5,b6)\
kp = vload2(k1/6,lkeyperm);\
a1 = lk(kp.x&0xff)^b(b1);\
a2 = lk((kp.x>>8)&0xff)^b(b2);\
a3 = lk((kp.x>>16))^b(b3);\
a4 = lk(kp.y&0xff)^b(b4);\
a5 = lk((kp.y>>8)&0xff)^b(b5);\
a6 = lk((kp.y>>16))^b(b6);

__kernel void store_hit(
						__global type *key_end,
						uint64_t hash,
						uint32_t slice_index,
						__global uint32_t *hits,
						__global uint32_t *hit_bool,
						__global type *keys,
						int where){
						
	int index = get_global_id(0);
    int size = get_global_size(0);
	
	*hit_bool = 1+where;  //Even if there is a race condition, it doesn't matter who wins.
					//no thread will try to write 0. Host only needs to know that
					//there was a hit
	
	hits[BATCH_SIZE*index+key_end[0]-ASCII_MIN_VAL] |= 1<<slice_index;
	
#if debugprints
	printf("/**********************************\n************* HIT *****************\n*************************************/\nHit was AMD in slice %d\n",slice_index);
	printf("Key end is values: %d %d\n",key_end[1], key_end[0]);
	printf("Key end is ascii: %c %c\n",key_end[1], key_end[0]);
	printf("I wrote it to hits index %d\n",BATCH_SIZE*index+key_end[0]-ASCII_MIN_VAL);
	printf("Where: %d\n",where);
	printf("Trip:");
	for(int j=0; j<1;j++){
		uint64_t faggot = hash;
		for(int i=0; i<10; i++){
			printf("%c",tr_sub[(faggot>>(6*i))&63]);
		}
		printf("\n");
		char key[7];
		key[6] = '\0';

		
		for(int a=0; a < 6; a++){
			int k = 0;
			int bit = 0;
			for(int b = 0; b < 7; b++){
				bit = (key(a*7+b)&(1<<slice_index))?1:0;
				k |= bit<<b;
			}
			key[a] = k;
		}
		printf("It appears the key was: %c%c%c%c%c%c\n",key[0],key[1],key[2],key[3],key[4],key[5]);
	}
#endif
}

__kernel void crypt25(__global type *keys,	//First 6 characters of each key
                      __global type *blocks,//Space to back-up hashes
					  __global type *gwa,	//Global Work Area for Quine-McCluskey's method
					  __global struct WDICT *g_dictpool,	//Dictionaries for the search
					  __global struct WDK * wdk_pool,		//Wdk entries
					  __global struct WDW * wdw_pool		//Words of all dictionary
											){	
	
    int index = get_global_id(0);
    int lindex = get_local_id(0);
    int size = get_global_size(0);
	

	
	/** ADDITIONAL PARAMETERS **/
	//Calculate additional parameters.
	//Performance would plunge if i had too many parameters... register spilling? That doesn't seem right.

	/** Regular parameters **/
	//should perhaps make sure it's nicely aligned?
	
	__global type *key_end;//Last two characters, shared across keys~
	
	key_end = &keys[56*size];
	
	
	/** Hit parameters **/
	//These parameters are used only in the case of an actual hit. Don't care
	//about performance in these cases.
	
	__global uint32_t *hit_bool;//saves the host from having to read the
								//entire hits array when there are no hits.
								
	__global uint32_t *hits;//One for each work item (here is the large memory usage of this program!)
	
	hit_bool = key_end + 2;
	hits = hit_bool + 1;
	
									

									
	//Keep block in registers while performing hash.
    __private type pblock[66];
	
	//Reserve ALL the shared memory for use by this block.
	__local int lmem[0x2000];
	
	//Put key expension matrix in local memory, noticeably faster on AMD gpus.
    __local int *lkeyperm = lmem;
	
	//Pointer to this thread's key slices.
	__local type *lkeys = (__local type *)(lmem+0x100+lindex);
	
    //Copy key expansion matrix to Local memory in a coalesced manner.
    for(int i = 0; i < 0x100/lsize; i++){
        int t = i*lsize+lindex;
		//Pack values in key expansion matrix to save memory accesses (local memory is slower than advertised, apparently)
        if(t<0x100)lkeyperm[t] = keyperm[t*3]|(keyperm[t*3+1]<<8)|(keyperm[t*3+2]<<16);
    }

#if asdasd
	if(index==0){
	//Copy key slices into Local memory.
    for(int i = 0; i < 56; i++){
		lk(i) = (testtrip[i/7]&(1<<(i%7)))?1:0;
    }
#else
	//Copy key slices into Local memory.
    for(int i = 0; i < 42; i++){
        lk(i) = key(i);
    }
	
	//Copy common end of key into local memory
	{
		lk(42) = (key_end[1]&(1<<0))?0xffffffff:0;
		lk(43) = (key_end[1]&(1<<1))?0xffffffff:0;
		lk(44) = (key_end[1]&(1<<2))?0xffffffff:0;
		lk(45) = (key_end[1]&(1<<3))?0xffffffff:0;
		lk(46) = (key_end[1]&(1<<4))?0xffffffff:0;
		lk(47) = (key_end[1]&(1<<5))?0xffffffff:0;
		lk(48) = (key_end[1]&(1<<6))?0xffffffff:0;
		lk(49) = (key_end[0]&(1<<0))?0xffffffff:0;
		lk(50) = (key_end[0]&(1<<1))?0xffffffff:0;
		lk(51) = (key_end[0]&(1<<2))?0xffffffff:0;
		lk(52) = (key_end[0]&(1<<3))?0xffffffff:0;
		lk(53) = (key_end[0]&(1<<4))?0xffffffff:0;
		lk(54) = (key_end[0]&(1<<5))?0xffffffff:0;
		lk(55) = (key_end[0]&(1<<6))?0xffffffff:0;
	}
#endif
    //Zero the blocks
    for(int i = 0; i < 64; i++){
        b(i) = 0;
    }
	
	//Perform the DES algorithm
	int2 kp = 0;
	int iterations = 25;
	int rounds_and_swapped = 8;
	int go = 1;
	type a1=0, a2=0, a3=0, a4=0, a5=0, a6=0;
	barrier(CLK_LOCAL_MEM_FENCE);
	while(go){
        xor_E(0, e0, e1, e2, e3, e4, e5)
        S1(40, 48, 54, 62)
        xor_E(6, e6, e7, e8, e9, e10, e11)
        S2(44, 59, 33, 49)
        xor_E(12,e12, e13, e14, e15, e16, e17)
        S3(55, 47, 61, 37)
        xor_E(18, e18, e19, e20, e21, e22, e23)
        S4(57, 51, 41, 32)
        xor_E(24, e24, e25, e26, e27, e28, e29)
        S5(39, 45, 56, 34)
        xor_E(30, e30, e31, e32, e33, e34, e35)
        S6(35, 60, 42, 50)
        xor_E(36, e36, e37, e38, e39, e40, e41)
        S7(63, 43, 53, 38)
        xor_E(42, e42, e43, e44, e45, e46, e47)
        S8(36, 58, 46, 52)

		if (rounds_and_swapped != 0x100){
			while(1){

                xor_E(48, e48, e49, e50, e51, e52, e53)
                S1(8, 16, 22, 30)
                xor_E(54, e54, e55, e56, e57, e58, e59)
                S2(12, 27, 1, 17)
                xor_E(60, e60, e61, e62, e63, e64, e65)
                S3(23, 15, 29, 5)
                xor_E(66, e66, e67, e68, e69, e70, e71)
                S4(25, 19, 9, 0)
                xor_E(72, e72, e73, e74, e75, e76, e77)
                S5(7, 13, 24, 2)
                xor_E(78, e78, e79, e80, e81, e82, e83)
                S6(3, 28, 10, 18)
                xor_E(84, e84, e85, e86, e87, e88, e89)
                S7(31, 11, 21, 6)
                xor_E(90, e90, e91, e92, e93, e94, e95)
                S8(4, 26, 14, 20)

				lkeyperm += 32;
				if (--rounds_and_swapped)break;
				lkeyperm -= (0x300 + 48)/3;
				rounds_and_swapped = 0x108;
				if (--iterations)continue;
				go=0;break;
			}
		}
		else{
			lkeyperm -= (0x300 - 48)/3;
			rounds_and_swapped = 8;
			iterations--;
		}
	}
	//Back-up blocks

    for(int i = 0; i < 64; i++){
        gb(i) = b(i);
    }
	/*
	 * Runtime host code will insert code running a preliminary comparison based on Quine-McCluskey's method here.
	 */
	 barrier(CLK_LOCAL_MEM_FENCE);
	 type x0,x1;
	{
		type x2,x3,x4,x5,x6,x7;
		
		//Defines printed to cmp.cl by synth.c implement the logic gates generated by quine mclusky's method.
		
		COMPARISON
		
		ASSEMBLY
		
	}
	//This barrier is needed because, depending on the type, we might
	//change width of the type used in local memory. which will cause
	//changed access pattern to local memory, so one warp racing ahead would
	//destroy for the rest.
	for(int a=0; a<64; a++)gwork_area(a)=work_area(a);
	//Mask detailing potential hits
	x0 = work_area(0);
	//Mask detailing actual hits that could
	//be determined by Quine-McCluskey's method
	x1 = work_area(1);
	barrier(CLK_LOCAL_MEM_FENCE);
	
	
	//Perform FP and matrix transmutation into local memory.
	//This trick lets us get all hashes into their own 64bit variable
	//in only o(nhash * log(bits)) time, rather than the naive o(nhash * bits)
	__local uint64_t *lblock = (__local uint64_t *)lmem;
	
	//Store a double word, buildt from two sources
	#define SDW(src1,src2,dest)\
	lb(dest) = gb(src2);\
	lb(dest) <<= 32;\
	lb(dest) |= gb(src1);
	//Store a double word, buildt from only one source as
	//the would-be second is either ignored or always 0.
	#define SW(src,dest)\
	lb(dest) = gb(src);
	
	//Perform FP and first step of matrix transmutation in one.
	{
		SDW(0x0E,0x1B,0x00)
		SDW(0x2E,0x3B,0x01)
		SDW(0x06,0x13,0x02)
		SDW(0x26,0x33,0x03)
		SDW(0x1F,0x1A,0x04)
		SDW(0x3F,0x3A,0x05)
		SDW(0x05,0x12,0x06)
		SDW(0x25,0x32,0x07)
		SDW(0x1E,0x0A,0x08)
		SDW(0x3E,0x2A,0x09)
		SDW(0x16,0x11,0x0A)
		SDW(0x36,0x31,0x0B)
		SDW(0x1D,0x09,0x0C)
		SDW(0x3D,0x29,0x0D)
		SDW(0x15,0x01,0x0E)
		SDW(0x35,0x21,0x0F)
		SDW(0x0D,0x08,0x10)
		SDW(0x2D,0x28,0x11)
		SDW(0x14,0x00,0x12)
		SDW(0x34,0x20,0x13)
		SDW(0x0C,0x19,0x14)
		SDW(0x2C,0x39,0x15)
		SDW(0x0B,0x18,0x18)
		SDW(0x2B,0x38,0x19)
		SDW(0x03,0x10,0x1A)
		SDW(0x23,0x30,0x1B)
		SW(0x04,0x16)
		SW(0x24,0x17)
		SW(0x1C,0x1C)
		SW(0x3C,0x1D)
		SW(0x02,0x1E)
		SW(0x22,0x1F)
	}
	
	//Rest of the transmutation
	uint64_t mask = 0xffffffffffffffff;
	uint32_t IC = 16;
	mask <<= 32;
	while(IC>0){
		uint64_t tmp = mask;
		mask >>= IC;
		mask ^= tmp;
		__local uint64_t *rblock = &lb(IC);
		uint32_t IK = 0;
		while(IK<32){
			uint64_t first = lb(IK);
			uint64_t second = rb(IK);
			uint64_t second_b = second;
			second <<= IC;
			second ^= first;
			second &= mask;
			first ^= second;
			lb(IK) = first;
			second >>= IC;
			second ^= second_b;
			rb(IK) = second;
			
			// Incrementing this way causes IK to skip a length of IC
			// every time it reaches a multiple of IC.
			IK+=IC;
			IK++;
			IK|=IC;
			IK^=IC;
		}
		IC >>= 1;
	}/*
	printf("Trips:\n");
	for(int j=0; j<32;j++){
		printf("%d: ",j);
		uint64_t faggot = lb(j);
		for(int i=0; i<10; i++){
			printf("%c",tr_sub[(faggot>>(6*i))&63]);
		}
		printf("\n");
	}*/
	//Move potential finds to appear sequentially from the start
	//preventing the search loop from going serial
	int potential_finds = 0;
	//potential finds are stored in Local memory, but are also
	//paired together with their old index which will be needed~
	for(type i = 0; i < 8*sizeof(type); i++){
		if(x0&((type)1<<i)){
			if(x1&((type)1<<i)){
				store_hit(key_end,lb(i),i,hits,hit_bool,keys,1);
				
			}
			else {
				gb(potential_finds) = i;
				
#if debugprints
if(*hit_bool)continue;
	printf("POTENTIAL Hit was AMD in slice %d\n",i);
	printf("Key end is ascii: %c %c\n",key_end[1], key_end[0]);
	printf("Trips:\n");
	for(int j=0; j<32;j++){
		printf("%d: ",j);
		uint64_t faggot = lb(j);
		for(int i=0; i<10; i++){
			printf("%c",tr_sub[(faggot>>(6*i))&63]);
		}
		printf("\n");
		char key[7];
		key[6] = '\0';

		
		for(int a=0; a < 6; a++){
			int k = 0;
			int bit = 0;
			for(int b = 0; b < 7; b++){
				bit = (key(a*7+b)&(1<<j))?1:0;
				k |= bit<<b;
			}
			key[a] = k;
		}
		printf("Key: %c%c%c%c%c%c\n",key[0],key[1],key[2],key[3],key[4],key[5]);
	}
#endif
				
				lb(potential_finds++)=lb(i);
				

			}	
		}
	}
	//Search for hits in the dictionary.
	for(potential_finds--;potential_finds>=0;potential_finds--){
		uint64_t item = lb(potential_finds);
		item &= 0x0fffffffffffffff;
		
		//Normalize
		uint64_t norm = normalise(item);
		
		//start search
		int hindex = gb(potential_finds);
		//Iterate over the valid dictionaries.
		for(int p = MIN_DICTPOOL; p < N_DICTPOOL; p++){
			__global struct WDICT *pd = &g_dictpool[p];
			
			int len = pd->len;
			
			//If there is a potential hit in this dictionary
			if(len > 0 && (gwork_area(p) & ((type)1<<hindex))){
				uint64_t m;
				uint32_t k = ((norm >> 6 * ((pd->pos - 1) + len - HASH_NCHARS))
							& ((1 << 6 * HASH_NCHARS) - 1));
				uint32_t ia, ib;
				uint32_t i, j;
				
				//Subtle hash function
				i = hash(k);
				//first check, is the hash in this dictionary?
				if(!(pd->bmhash[i / 32] & (1u << (i & (31u)))))
					continue;
				//Perform binary search for the primary search key
				int found_first = 0;
				for(ia = 0, ib = pd->nwdk;
						ib > 0;
						ib >>= 1){
					
					uint32_t x,y;
					i = ia + (ib >> 1);
					x = k;
					y = wdk_pool[i+pd->wdk].k;
					if(x==y){
						found_first++;
						break;
					}
					else if(x>y){
						ia = i + 1;
						ib--;
					}
				}
				if(!found_first)continue;
				//found first, perform binary search for secondary key.
				m = (norm >> 6 * (pd-> pos - 1)) & ((1ULL << 6 * pd->len) - 1);
				
				ia = i?(wdk_pool[i-1+pd->wdk].i):(0);

				for(ib = wdk_pool[i+pd->wdk].i - ia;
						ib > 0;
						ib >>= 1){
					uint64_t x,y;
					i = ia + (ib >> 1);
					x = m;
					y = normalise(wdw_pool[i+pd->words].w);
					//printf("ib: %d, x: %I64u, y: %I64u\n",ib, x,y);
					if(x == y){
						//We found the secondary key!
						//This is such a rare case that the remainder of the search
						//is not optimized, and is sequential.
						x = ((item >> 6 * (pd->pos - 1)) & ((1ULL << 6 * pd->len) - 1));
						//printf("Found secondary key.\nx: %I64u\nitem: %I64u\npos: %d\nlen: %d\n",x,item,pd->pos, pd->len);
						for(j = i;
								j < pd->nwords
								 && normalise(wdw_pool[pd->words+j].w)==m;
								j++)
						{
							uint64_t word = wdw_pool[pd->words+j].w;
							uint64_t xi = xpize(x,wdw_pool[pd->words+j].xp,len);
							if(xi == word){
								store_hit(key_end,item,hindex,hits,hit_bool,keys, 0);
							}
						}
						
						for(j = i-1;
								j >= 0 &&  j < pd->nwords
								 && normalise(wdw_pool[pd->words+j].w)==m;
								j--)
						{
							uint64_t word = wdw_pool[pd->words+j].w;
							uint64_t xi = xpize(x,wdw_pool[pd->words+j].xp,len);
							if(xi == word){
								store_hit(key_end,item,hindex,hits,hit_bool,keys, 0);
							}
						}
						break;
					}
					
					if( x > y )
					{
						ia = i + 1;
						ib--;
					}
				}
			}
		}
	}
	#if asdasd
	}
	#endif
}

//Performs an increment on the last key byte, which is shared.
//Performing this in the main kernel instead leads to a race
//condition where warps / wavefronts that finish after the first
//will read the wrong value and store information about any hits
//made wrongly.
//The penalty for the extra kernel call is close to zero.
__kernel void inc_key(__global type *keys){
    int index = get_global_id(0);
    int size = get_global_size(0);
	
	__global type *key_end;
	
	key_end = &keys[56*size];
    if(index==0){
        uint32_t temp = key_end[0]+1;
		temp = 	temp>ASCII_MAX_VAL
				?	ASCII_MIN_VAL
				:	temp;
		key_end[0] = temp;
    }
}

