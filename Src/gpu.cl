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

#define lsize 512

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

/*
 * defines to save typing and make kernel code more readable
 */
#define key(x) keys[(x)*size+index]
#define lk(x)  lkeys[(x)*lsize]
#define temps(x) temporaries[(x)*size+index]
#define lb(x) lblock[(x)*lsize]
#define rb(x) rblock[(x)*lsize]
#define work_area(x) lmem[(x)*lsize+lindex]
#define gwork_area(x) gwa[(x)*size+index]
#define gb(x)	b##x

#define SWAP(a,b) \
{ \
	uint32_t swap = a; \
	a = b; \
	b = swap; \
}

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
	
	//Index into the hits array to find this slice of this work item's reserved area.
	index *= BATCH_SIZE*32;
	index += slice_index*BATCH_SIZE;
	//Find the first free spot to put the hit in. Usually all will be free~
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
	//__local uint32_t lmem[16*512];
	//__local uint32_t *lblock = lmem+lindex;
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
	
	uint32_t kreg0, kreg1, kreg2, kreg3, kreg4, kreg5, kreg6, kreg7, kreg8, kreg9, kreg10, kreg11, kreg12, kreg13, kreg14, kreg15, kreg16, kreg17, kreg18, kreg19, kreg20, kreg21, kreg22, kreg23, kreg24, kreg25, kreg26, kreg27, kreg28, kreg29, kreg30, kreg31, kreg32, kreg33, kreg34;
	
	kreg0 = key(21+0);
	kreg1 = key(21+1);
	kreg2 = key(21+2);
	kreg3 = key(21+3);
	kreg4 = key(21+4);
	kreg5 = key(21+5);
	kreg6 = key(21+6);
	kreg7 = key(21+7);
	kreg8 = key(21+8);
	kreg9 = key(21+9);
	kreg10 = key(21+10);
	kreg11 = key(21+11);
	kreg12 = key(21+12);
	kreg13 = key(21+13);
	kreg14 = key(21+14);
	kreg15 = key(21+15);
	kreg16 = key(21+16);
	kreg17 = key(21+17);
	kreg18 = key(21+18);
	kreg19 = key(21+19);
	kreg20 = key(21+20);
	
	//Set the last two characters
	uint32_t key1, key2, key_index;
	key_index = *key_end_index;
	key1 = key_ends[key_index*2+0];
	key2 = key_ends[key_index*2+1];
	
	kreg21 = -((key1>>0)&1);
	kreg22 = -((key1>>1)&1);
	kreg23 = -((key1>>2)&1);
	kreg24 = -((key1>>3)&1);
	kreg25 = -((key1>>4)&1);
	kreg26 = -((key1>>5)&1);
	kreg27 = -((key1>>6)&1);
	kreg28 = -((key2>>0)&1);
	kreg29 = -((key2>>1)&1);
	kreg30 = -((key2>>2)&1);
	kreg31 = -((key2>>3)&1);
	kreg32 = -((key2>>4)&1);
	kreg33 = -((key2>>5)&1);
	kreg34 = -((key2>>6)&1);

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
	
	if(x0){
	
		TRANSPOSE
		
		/*
		 *	Place any hashes that pass through initial comparison sequentially first such that if multiple threads have some
		 *	they will be processed in parallel.
		 */
		uint64_t hashes[] = {
			hash0, hash1, hash2, hash3, hash4, hash5, hash6, hash7, hash8, hash9, hash10, hash11, hash12, hash13, hash14, hash15, hash16, hash17, hash18, hash19, hash20, hash21, hash22, hash23, hash24, hash25, hash26, hash27, hash28, hash29, hash30, hash31, hash32, hash33, hash34, hash35, hash36, hash37, hash38, hash39, hash40, hash41, hash42, hash43, hash44, hash45, hash46, hash47, hash48, hash49, hash50, hash51, hash52, hash53, hash54, hash55, hash56, hash57, hash58, hash59, hash60, hash61, hash62, hash63
		};
		uint64_t hashes_i[32];
		uint32_t count = 0;
		
		for(uint32_t i = 0; i < sizeof(hashes)/sizeof(*hashes); i++){
			if(x0&((uint32_t)1<<i)){
				if(x1&((uint32_t)1<<i)){
					store_hit(hashes[i], i, hits, hit_bool, key1, key2);
				}
				else {
					hashes_i[count] = i;
					hashes[count++]=hashes[i];
				}	
			}
		}
		
		//Search for hits in the dictionary.
		for(int it = 0; it<count; it++){
			uint64_t item = hashes[it];
			item &= 0x0fffffffffffffff;
			
			//Normalize
			uint64_t norm = normalise(item);
			
			//start search
			int hindex = hashes_i[it];
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
					m = (norm >> 6 * (pd-> pos - 1)) & ((((uint64_t)1) << 6 * pd->len) - 1);
					
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
							x = ((item >> 6 * (pd->pos - 1)) & ((((uint64_t)1) << 6 * pd->len) - 1));
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

//Since opencl does not provide a method to set a memory object on the GPU to 0, i have to launch a kernel to do this.
__kernel void clear_hits(__global uint32_t *hits, uint32_t hits_size){
    int index = get_global_id(0);
    int size = get_global_size(0);
	hits_size /= sizeof(uint32_t);

	for(int i = 0; index + size*i < hits_size; i++){
		hits[56*size + 2 + index + size*i] = 0;
	}
	if(index == 0){
		hits[56*size + 1] = 0;
	}
	
}