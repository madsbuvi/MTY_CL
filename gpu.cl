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
 */
#define key(x) keys[(x)*size+index]
#define lk(x)  lkeys[(x)*lsize]
#define gb(x) blocks[(x)*size+index]
#define lb(x) lblock[(x)*lsize + lindex]
#define rb(x) rblock[(x)*lsize]
#define work_area(x) lmem[(x)*lsize+lindex]
#define gwork_area(x) gwa[(x)*size+index]

#define SWAP(a,b) \
{ \
	type swap = a; \
	a = b; \
	b = swap; \
}

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
	printf("/**********************************\n************* HIT *****************\n*************************************/\nHit was made in slice %d\n",slice_index);
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
    //__private type pblock[66];
	type b0;
	type b1;
	type b2;
	type b3;
	type b4;
	type b5;
	type b6;
	type b7;
	type b8;
	type b9;
	type b10;
	type b11;
	type b12;
	type b13;
	type b14;
	type b15;
	type b16;
	type b17;
	type b18;
	type b19;
	type b20;
	type b21;
	type b22;
	type b23;
	type b24;
	type b25;
	type b26;
	type b27;
	type b28;
	type b29;
	type b30;
	type b31;
	type b32;
	type b33;
	type b34;
	type b35;
	type b36;
	type b37;
	type b38;
	type b39;
	type b40;
	type b41;
	type b42;
	type b43;
	type b44;
	type b45;
	type b46;
	type b47;
	type b48;
	type b49;
	type b50;
	type b51;
	type b52;
	type b53;
	type b54;
	type b55;
	type b56;
	type b57;
	type b58;
	type b59;
	type b60;
	type b61;
	type b62;
	type b63;
	
	
	
	//Reserve ALL the shared memory for use by this block.
	__local int lmem[0x2000];
	
	//Put key expension matrix in local memory, noticeably faster on AMD gpus.
    __local int *lkeyperm = lmem;
	
	//Pointer to this thread's key slices.
	__local type *lkeys = (__local type *)(lmem+lindex);
	

	//Copy key slices into Local memory.
    for(int i = 0; i < 35; i++){
        lk(i) = key(i+21);
    }
	
	//Copy common end of key into local memory
	{
		lk(21) = (key_end[1]&(1<<0))?0xffffffff:0;
		lk(22) = (key_end[1]&(1<<1))?0xffffffff:0;
		lk(23) = (key_end[1]&(1<<2))?0xffffffff:0;
		lk(24) = (key_end[1]&(1<<3))?0xffffffff:0;
		lk(25) = (key_end[1]&(1<<4))?0xffffffff:0;
		lk(26) = (key_end[1]&(1<<5))?0xffffffff:0;
		lk(27) = (key_end[1]&(1<<6))?0xffffffff:0;
		lk(28) = (key_end[0]&(1<<0))?0xffffffff:0;
		lk(29) = (key_end[0]&(1<<1))?0xffffffff:0;
		lk(30) = (key_end[0]&(1<<2))?0xffffffff:0;
		lk(31) = (key_end[0]&(1<<3))?0xffffffff:0;
		lk(32) = (key_end[0]&(1<<4))?0xffffffff:0;
		lk(33) = (key_end[0]&(1<<5))?0xffffffff:0;
		lk(34) = (key_end[0]&(1<<6))?0xffffffff:0;
	}
    //Zero the blocks
	{
		b0 = 0;
		b1 = 0;
		b2 = 0;
		b3 = 0;
		b4 = 0;
		b5 = 0;
		b6 = 0;
		b7 = 0;
		b8 = 0;
		b9 = 0;
		b10 = 0;
		b11 = 0;
		b12 = 0;
		b13 = 0;
		b14 = 0;
		b15 = 0;
		b16 = 0;
		b17 = 0;
		b18 = 0;
		b19 = 0;
		b20 = 0;
		b21 = 0;
		b22 = 0;
		b23 = 0;
		b24 = 0;
		b25 = 0;
		b26 = 0;
		b27 = 0;
		b28 = 0;
		b29 = 0;
		b30 = 0;
		b31 = 0;
		b32 = 0;
		b33 = 0;
		b34 = 0;
		b35 = 0;
		b36 = 0;
		b37 = 0;
		b38 = 0;
		b39 = 0;
		b40 = 0;
		b41 = 0;
		b42 = 0;
		b43 = 0;
		b44 = 0;
		b45 = 0;
		b46 = 0;
		b47 = 0;
		b48 = 0;
		b49 = 0;
		b50 = 0;
		b51 = 0;
		b52 = 0;
		b53 = 0;
		b54 = 0;
		b55 = 0;
		b56 = 0;
		b57 = 0;
		b58 = 0;
		b59 = 0;
		b60 = 0;
		b61 = 0;
		b62 = 0;
		b63 = 0;
	}
	//Perform the DES algorithm
	type a1=0, a2=0, a3=0, a4=0, a5=0, a6=0;
	barrier(CLK_LOCAL_MEM_FENCE);
	int iterations = 25;
	while(iterations--){
		SWAP(b0,b32);
		SWAP(b1,b33);
		SWAP(b2,b34);
		SWAP(b3,b35);
		SWAP(b4,b36);
		SWAP(b5,b37);
		SWAP(b6,b38);
		SWAP(b7,b39);
		SWAP(b8,b40);
		SWAP(b9,b41);
		SWAP(b10,b42);
		SWAP(b11,b43);
		SWAP(b12,b44);
		SWAP(b13,b45);
		SWAP(b14,b46);
		SWAP(b15,b47);
		SWAP(b16,b48);
		SWAP(b17,b49);
		SWAP(b18,b50);
		SWAP(b19,b51);
		SWAP(b20,b52);
		SWAP(b21,b53);
		SWAP(b22,b54);
		SWAP(b23,b55);
		SWAP(b24,b56);
		SWAP(b25,b57);
		SWAP(b26,b58);
		SWAP(b27,b59);
		SWAP(b28,b60);
		SWAP(b29,b61);
		SWAP(b30,b62);
		SWAP(b31,b63);
		DES
	}
	
	//Back-up blocks
	{
		gb(0) = b0;
		gb(1) = b1;
		gb(2) = b2;
		gb(3) = b3;
		gb(4) = b4;
		gb(5) = b5;
		gb(6) = b6;
		gb(7) = b7;
		gb(8) = b8;
		gb(9) = b9;
		gb(10) = b10;
		gb(11) = b11;
		gb(12) = b12;
		gb(13) = b13;
		gb(14) = b14;
		gb(15) = b15;
		gb(16) = b16;
		gb(17) = b17;
		gb(18) = b18;
		gb(19) = b19;
		gb(20) = b20;
		gb(21) = b21;
		gb(22) = b22;
		gb(23) = b23;
		gb(24) = b24;
		gb(25) = b25;
		gb(26) = b26;
		gb(27) = b27;
		gb(28) = b28;
		gb(29) = b29;
		gb(30) = b30;
		gb(31) = b31;
		gb(32) = b32;
		gb(33) = b33;
		gb(34) = b34;
		gb(35) = b35;
		gb(36) = b36;
		gb(37) = b37;
		gb(38) = b38;
		gb(39) = b39;
		gb(40) = b40;
		gb(41) = b41;
		gb(42) = b42;
		gb(43) = b43;
		gb(44) = b44;
		gb(45) = b45;
		gb(46) = b46;
		gb(47) = b47;
		gb(48) = b48;
		gb(49) = b49;
		gb(50) = b50;
		gb(51) = b51;
		gb(52) = b52;
		gb(53) = b53;
		gb(54) = b54;
		gb(55) = b55;
		gb(56) = b56;
		gb(57) = b57;
		gb(58) = b58;
		gb(59) = b59;
		gb(60) = b60;
		gb(61) = b61;
		gb(62) = b62;
		gb(63) = b63;
	}

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
	}
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
	printf("POTENTIAL Hit was made in slice %d\n",i);
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
	#if 0
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

