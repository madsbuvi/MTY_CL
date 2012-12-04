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
#define printx(a) if(index == 0)printf(#a ": %x\n",a);
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
#define BITS 8*sizeof(uint32_t)

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

#endif
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

/*
 * defines to save typing and make kernel code more readable
 */
#define key(x) keys[(x)*size+index]
#define lk(x)  lkeys[(x)*lsize]
#define temps(x) temporaries[(x)*size+index]
#define lb(x) lblock[(x)*lsize + lindex]
#define rb(x) rblock[(x)*lsize]
#define work_area(x) lmem[(x)*lsize+lindex]
#define gwork_area(x) gwa[(x)*size+index]

#define SWAP(a,b) \
{ \
	uint32_t swap = a; \
	a = b; \
	b = swap; \
}



void store_hit(
						uint64_t hash, //Was included for debug prints, TODO: Remove
						uint32_t slice_index,
						__global uint32_t *hits,
						__global uint32_t *hit_bool,
						uint32_t key1, uint32_t key2){
						
	int index = get_global_id(0);
    int size = get_global_size(0);
	
	*hit_bool = 1;  //Even if there is a race condition, it doesn't matter who wins.
					//no thread will try to write 0. Host only needs to know that
					//there was a hit
	
	//Find the first free spot to put the hit in. Usually all will be free~
	index *= BATCH_SIZE*32;
	index += slice_index*BATCH_SIZE;
	while(hits[index])index++;
	hits[index] = (key2<<8)|key1;
}

__kernel void crypt25(__global uint32_t *keys,	//First 6 characters of each key, first 3 not really used
                      __global uint32_t *key_ends,//List of all possible last 2 characters of each key
					  __global uint32_t *gwa,	//Global Work Area for Quine-McCluskey's method
					  __global struct WDICT *g_dictpool,	//Dictionaries for the search
					  __global struct WDK * wdk_pool,		//Wdk entries
					  __global struct WDW * wdw_pool		//Words of all dictionaries
											){	
	
    int index = get_global_id(0);
    int lindex = get_local_id(0);
    int size = get_global_size(0);
	


	
	/** ADDITIONAL PARAMETERS **/
	//Calculate additional parameters.
	//Performance would plunge if i had too many parameters... register spilling? That doesn't seem right...

	/** Regular parameters **/
	__global uint32_t *key_end_index;
	
	key_end_index = keys + 56*size;
	
	
	/** Hit parameters **/
	//These parameters are used only in the case of an actual hit. Don't care
	//about performance in these cases so i don't care to make them nicely aligned to anything
	
	__global uint32_t *hit_bool;//saves the host from having to read the
								//entire hits array when there are no hits.
								
	__global uint32_t *hits;//One for each work item (here is the large memory usage of this program!)
	
	hit_bool = key_end_index + 1;
	hits = hit_bool + 1;
	
									
	//Keep block (ciphertext) in registers while performing hash.
	uint32_t b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11, b12, b13, b14, b15, b16, b17, b18, b19, b20, b21, b22, b23, b24, b25, b26, b27, b28, b29, b30, b31, b32, b33, b34, b35, b36, b37, b38, b39, b40, b41, b42, b43, b44, b45, b46, b47, b48, b49, b50, b51, b52, b53, b54, b55, b56, b57, b58, b59, b60, b61, b62, b63;
	
	
	
	//Reserve ALL the shared memory for use by this block.
	__local int lmem[0x2000];
	
	//Pointer to this thread's key slices.
	__local uint32_t *lkeys = (__local uint32_t *)(lmem+lindex);
	
	//Copy host-determined key slice
	for(int i = 0; i < 21; i++){
		
		lk(i) = key(i+21);
	}
	
	//Set the last two characters
	uint32_t key1, key2, key_index;
	key_index = *key_end_index;
	key1 = key_ends[key_index*2+0];
	key2 = key_ends[key_index*2+1];
	
	lk(21) = -((key1>>0)&1);
	lk(22) = -((key1>>1)&1);
	lk(23) = -((key1>>2)&1);
	lk(24) = -((key1>>3)&1);
	lk(25) = -((key1>>4)&1);
	lk(26) = -((key1>>5)&1);
	lk(27) = -((key1>>6)&1);
	lk(28) = -((key2>>0)&1);
	lk(29) = -((key2>>1)&1);
	lk(30) = -((key2>>2)&1);
	lk(31) = -((key2>>3)&1);
	lk(32) = -((key2>>4)&1);
	lk(33) = -((key2>>5)&1);
	lk(34) = -((key2>>6)&1);

    //Zero the blocks
	b0 = 0; b1 = 0; b2 = 0; b3 = 0; b4 = 0; b5 = 0; b6 = 0; b7 = 0; b8 = 0; b9 = 0; b10 = 0; b11 = 0; b12 = 0; b13 = 0; b14 = 0; b15 = 0; b16 = 0; b17 = 0; b18 = 0; b19 = 0; b20 = 0; b21 = 0; b22 = 0; b23 = 0; b24 = 0; b25 = 0; b26 = 0; b27 = 0; b28 = 0; b29 = 0; b30 = 0; b31 = 0; b32 = 0; b33 = 0; b34 = 0; b35 = 0; b36 = 0; b37 = 0; b38 = 0; b39 = 0; b40 = 0; b41 = 0; b42 = 0; b43 = 0; b44 = 0; b45 = 0; b46 = 0; b47 = 0; b48 = 0; b49 = 0; b50 = 0; b51 = 0; b52 = 0; b53 = 0; b54 = 0; b55 = 0; b56 = 0; b57 = 0; b58 = 0; b59 = 0; b60 = 0; b61 = 0; b62 = 0; b63 = 0;
	//Perform the DES algorithm
	uint32_t a1=0, a2=0, a3=0, a4=0, a5=0, a6=0;
	barrier(CLK_LOCAL_MEM_FENCE);
	int iterations = 25;
	while(iterations--){
		SWAP(b0,b32); SWAP(b1,b33); SWAP(b2,b34); SWAP(b3,b35); SWAP(b4,b36); SWAP(b5,b37); SWAP(b6,b38); SWAP(b7,b39); SWAP(b8,b40); SWAP(b9,b41); SWAP(b10,b42); SWAP(b11,b43); SWAP(b12,b44); SWAP(b13,b45); SWAP(b14,b46); SWAP(b15,b47); SWAP(b16,b48); SWAP(b17,b49); SWAP(b18,b50); SWAP(b19,b51); SWAP(b20,b52); SWAP(b21,b53); SWAP(b22,b54); SWAP(b23,b55); SWAP(b24,b56); SWAP(b25,b57); SWAP(b26,b58); SWAP(b27,b59); SWAP(b28,b60); SWAP(b29,b61); SWAP(b30,b62); SWAP(b31,b63);
		
		DES //des.cl
	}
	
	barrier(CLK_LOCAL_MEM_FENCE);
	uint32_t x0,x1,x2,x3,x4,x5,x6,x7;
		
	//Defines printed to cmp.cl by synth.c implement the logic gates generated by quine mclusky's method.
	
	COMPARISON
	
	ASSEMBLY
	
	gwork_area( 2 ) = work2;
	gwork_area( 3 ) = work3;
	gwork_area( 4 ) = work4;
	gwork_area( 5 ) = work5;
	gwork_area( 6 ) = work6;
	gwork_area( 7 ) = work7;
	gwork_area( 8 ) = work8;
	gwork_area( 9 ) = work9;
	gwork_area( 10 ) = work10;
	gwork_area( 11 ) = work11;
	gwork_area( 12 ) = work12;
	gwork_area( 13 ) = work13;
	gwork_area( 14 ) = work14;
	gwork_area( 15 ) = work15;
	gwork_area( 16 ) = work16;
	gwork_area( 17 ) = work17;
	gwork_area( 18 ) = work18;
	gwork_area( 19 ) = work19;
	gwork_area( 20 ) = work20;
	
	//Mask detailing potential hits
	x0 = work0;
	//Mask detailing actual hits that could be determined
	x1 = work1;
	
	
	//Perform FP and matrix transmutation into local memory.
	//This trick lets us get all hashes into their own 64bit variable
	//in only o(nhash * log(bits)) time, rather than the naive o(nhash * bits)
	__local uint64_t *lblock = (__local uint64_t *)lmem;
	
	//Store a double word, buildt from two sources
	#define SDW(src1,src2,dest)\
	lb(dest) = b##src2;\
	lb(dest) <<= 32;\
	lb(dest) |= b##src1;
	//Store a double word, buildt from only one source as
	//the would-be second is either ignored or always 0.
	#define SW(src,dest)\
	lb(dest) = b##src;

	//Perform FP and first step of matrix transmutation in one.
	//But only do it if we could not prove that there won't be a hit.
	if(x0){
		{
			SDW(14, 27, 0);
			SDW(46, 59, 1);
			SDW(6, 19, 2);
			SDW(38, 51, 3);
			SDW(31, 26, 4);
			SDW(63, 58, 5);
			SDW(5, 18, 6);
			SDW(37, 50, 7);
			SDW(30, 10, 8);
			SDW(62, 42, 9);
			SDW(22, 17, 10);
			SDW(54, 49, 11);
			SDW(29, 9, 12);
			SDW(61, 41, 13);
			SDW(21, 1, 14);
			SDW(53, 33, 15);
			SDW(13, 8, 16);
			SDW(45, 40, 17);
			SDW(20, 0, 18);
			SDW(52, 32, 19);
			SDW(12, 25, 20);
			SDW(44, 57, 21);
			SDW(11, 24, 24);
			SDW(43, 56, 25);
			SDW(3, 16, 26);
			SDW(35, 48, 27);
			SW(4, 22);
			SW(36, 23);
			SW(28, 28);
			SW(60, 29);
			SW(2, 30);
			SW(34, 31);

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

		for(uint32_t i = 0; i < 8*sizeof(uint32_t); i++){
			if(x0&((uint32_t)1<<i)){
				if(x1&((uint32_t)1<<i)){
					store_hit(lb(i), i, hits, hit_bool, key1, key2);
				}
				else {
					gwork_area(21+potential_finds) = i;
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
			int hindex = gwork_area(21+potential_finds);
			//Iterate over the valid dictionaries.
			for(int p = MIN_DICTPOOL; p < N_DICTPOOL; p++){
				__global struct WDICT *pd = &g_dictpool[p];
				
				int len = pd->len;
				
				//If there is a potential hit in this dictionary
				if(len > 0 && (gwork_area(p) & ((uint32_t)1<<hindex))){
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
									store_hit(item, hindex, hits, hit_bool, key1, key2);
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
									store_hit(item, hindex, hits, hit_bool, key1, key2);
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
	}
}

//Used as a global synchronization point. If this was done in the crypt25 kernel there would be no way
//to guarantee this wasn't done before one or more wavefronts ever wrote their own last keys. Although it seems unlikely.
__kernel void inc_key_offset(__global uint32_t *keys){
    int index = get_global_id(0);
	int size = get_global_size(0);
	if(!index){
		__global uint32_t *key_end_index;
		key_end_index = keys + 56*size;
		key_end_index[0]++;
	}
}