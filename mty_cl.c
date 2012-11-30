/***********************************************************************
 *
 *	file: mty_cl.c
 *
 *	Host side of the OpenCL implementation of mty cl
 *
 *	Written by Mads Buvik Sandvei (https://github.com/madsbuvi/, madsbuvi@stud.ntnu.no)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include "desconst.h"
#include "mty_cl.h"
#include "cl_util.h"
#include "DES_std.h"
#include "windows.h"
#include "wdict.h"
#include "log.h"
#include "crypt3.h"
#include "key_cl.h"

//Structures used by MTY for matching tripcodes
void *dictpool,*wdk_pool,*wdw_pool;
int32_t dictpool_size, wdksize, wdwsize, min_dictpool, n_dictpool;

//Sources
char *source_crypt, *source_sboxdef, *source_cmp,  *source_des;
size_t len_crypt, len_sboxdef, len_cmp, len_wdict_config, len_des;

//Will contain a list of what ciphertext bits are found in registers and which must be found via
//LDS/Shared memory.
char B_reg[64];
int cipher_registers;
//CL source of the above, list of defines.
char B_cl[2048];

//generated
char salt_chars[] =  ".............................................../0123456789ABCDEFGABCDEFGHIJKLMNOPQRSTUVWXYZabcdefabcdefghijklmnopqrstuvwxyz.....................................................................................................................................";

//Initialize key permutation
void init_ksp(uint32_t *KSp){
	uint32_t *k;
	int32_t round, index, bit;
	int32_t p, q, s;
	k = KSp;

	s = 0;
	for (round = 0; round < 16; round++) {
		s += DES_ROT[round];
		for (index = 0; index < 48; index++) {
			p = DES_PC2[index];
			q = p < 28 ? 0 : 28;
			p += s;
			while (p >= 28) p -= 28;
			bit = DES_PC1[p + q];
			bit ^= 070;
			bit -= bit >> 3;
			bit = 55 - bit;
			*k++ = bit;
		}
	}
}

//Sets a key to all slices.
void set_key(uint32_t *key, uint8_t *plaintext, int32_t length){
	uint32_t mask;
	uint32_t zero,one,a,b;
	zero = 0;
	one = ~zero;
	for(a=6,b=0;b<length;b++){
		mask = 1<<6;
		key[a]=(plaintext[b]&mask)>0?one:zero;a--;mask>>=1;
		key[a]=(plaintext[b]&mask)>0?one:zero;a--;mask>>=1;
		key[a]=(plaintext[b]&mask)>0?one:zero;a--;mask>>=1;
		key[a]=(plaintext[b]&mask)>0?one:zero;a--;mask>>=1;
		key[a]=(plaintext[b]&mask)>0?one:zero;a--;mask>>=1;
		key[a]=(plaintext[b]&mask)>0?one:zero;a--;mask>>=1;
		key[a]=(plaintext[b]&mask)>0?one:zero;a--;mask>>=1;
		a+=14;
	}
}

//Sets a key to the given slice
//Presumes that the slice is already cleared
void set_key_line(uint32_t *key, uint8_t *plaintext, uint32_t line, int32_t length){
	uint32_t mask;
	uint32_t zero,one,a,b;
	line = 1u << line;
	zero = 0;
	one = ~zero;
	for(a=6,b=0;b<length;b++){
		mask = 1<<6;
		key[a]|=line&((plaintext[b]&mask)>0?one:zero);a--;mask>>=1;
		key[a]|=line&((plaintext[b]&mask)>0?one:zero);a--;mask>>=1;
		key[a]|=line&((plaintext[b]&mask)>0?one:zero);a--;mask>>=1;
		key[a]|=line&((plaintext[b]&mask)>0?one:zero);a--;mask>>=1;
		key[a]|=line&((plaintext[b]&mask)>0?one:zero);a--;mask>>=1;
		key[a]|=line&((plaintext[b]&mask)>0?one:zero);a--;mask>>=1;
		key[a]|=line&((plaintext[b]&mask)>0?one:zero);a--;mask>>=1;
		a+=14;
	}
}

void reset_salt(char *E){
	uint32_t a;
	for(a = 0; a<48;a++){
		E[a] = DES_E[a];
		E[a+48] = DES_E[a] + 32;
	}
}

int is_register(int cipher_bit){
	return 1;
}

void cl_set_salt(uint8_t *salt_in, char *E_cl)
{
	char E[96];
	reset_salt(E);
	uint8_t salt[2] = {salt_chars[salt_in[0]],salt_chars[salt_in[1]]};
	uint32_t temp;
	int32_t c,i,j;
	for(i=0; i<2 ;i++){
		c = salt[i];
		if(c>'Z')c-=6;
		if(c>'9')c-=7;
		c-='.';
		for(j=0; j<6; j++){
			if((c>>j)&1){
				temp = E[6*i+j];
				E[6*i+j] = E[6*i+j+24];
				E[6*i+j+24] = temp;

				temp = E[6*i+j+48];
				E[6*i+j+48] = E[6*i+j+24+48];
				E[6*i+j+24+48] = temp;
			}
		}
	}
	
	//Update the opencl source code
	for(i=0; i<96; i++){
		E_cl += sprintf(E_cl, "#define e%d b%d\n",i,E[i]);
	}
}

//Change positions of different key bits such that the GPU can utilize coalesced access patterns
//I.E. the nth bit of every key are to be neighbors.
void coalesc_keys(uint32_t *keys_plain, uint32_t *keys_coalesced, uint32_t n){
	int32_t a,b;
    for(a=0; a<56; a++){
        for(b=0; b<n; b++){
            keys_coalesced[b]=keys_plain[a+b*56];
        }
        keys_coalesced+=n;
    }
}

void examine(uint32_t hits, cl_key_char *keys){
	if(!hits)return;
	uint8_t key[9];
	int i;
	for(i = 0; i < 6; i++)key[i] = keys[i].key;
	char salt[2] = {salt_chars[key[1]],salt_chars[key[2]]};
	key[6] = hits&0xff;
	key[7] = hits/0x100;
	key[8] = cl_key_get_last(key[6], key[7]);
	cl_key_make_safe(key);
	char *hash = crypt(key, salt);
	assert(hash);
	log_print(1, hash+3, key);
	free(hash);
}

void cl_setup_shared_key(cl_key_char *key, char *cl){
	int i,j;
	for(i = 0; i < 8; i++){
		cl += sprintf(cl, "#define key%d_key %d\n", i, key[i].key);
		cl += sprintf(cl, "#define key%d_type %d\n", i, key[i].type);
		for(j = 0; j < 7; j++){
			cl += sprintf(cl, "#define k%d %s\n", i*7+j, (key[i].key&(1<<j))?"0xffffffff":"0");
		}
	}
	
}

#define BATCH_SIZE 100

/*
 * Infinitely searches random ASCII keys
 */
int do_search(int gpu){
	cl_key_char shared_key[9];
    cl_device_id device_id = NULL;
	cl_context context = NULL;
	cl_command_queue command_queue = NULL;
	cl_mem keys_and_extra_gpu, blocks_gpu, wa_gpu, g_dictpool_gpu,
	wdk_gpu, wdw_gpu;
	cl_program program = NULL;
	cl_kernel kernel = NULL;
	cl_kernel inc_kernel = NULL;
	cl_int ret;
	uint8_t salt[2];
	int32_t a,b,c;
	
	//Will contain a list of defines to pass to the opencl compiler
	//Instead of having another level of indirection with the extension matrix, i'll just
	//hardcode it and recompile when it needs to change.
	//So long as recompilation is rare enough, this is rewarding depsite the need for recompilation.
	char E_cl[2048];
	//Will contain defines on the bits of a shared key
	char K_cl[2048*2];
	//Will contain a few configurations related to wdict
	char wdict_config[1024];
	
	cl_key_reset(shared_key, 0);
	salt[0] = salt_chars[shared_key[1].key];
	salt[1] = salt_chars[shared_key[2].key];
	cl_set_salt(salt, E_cl);
	cl_setup_shared_key(shared_key, K_cl);
	
	printf("Shared key is %x %x %x\n", shared_key[0].key, shared_key[1].key, shared_key[2].key);
	
	len_wdict_config = sprintf(wdict_config,
			"#define MIN_DICTPOOL %d\n"
			"#define N_DICTPOOL %d\n"
			"#define BATCH_SIZE %d\n",
			min_dictpool, n_dictpool, BATCH_SIZE);

	/* Get Platform and Device Info */
	init_cl_gpu_specific(gpu, &device_id, &context, & command_queue);

	/* Create Kernel Program from the source */
	const char *sources[] = {K_cl, E_cl, source_des, wdict_config, source_sboxdef, source_cmp, source_crypt};
	const size_t lengths[] = {strlen(K_cl), strlen(E_cl), len_des, len_wdict_config, len_sboxdef, len_cmp, len_crypt};

	HandleErrorPar(program = clCreateProgramWithSource(context, sizeof(lengths)/sizeof(const size_t), sources,
	lengths, HANDLE_ERROR));
	
	/* Build Kernel Program */
	ret = clBuildProgram(program, 1, &device_id, "-cl-opt-disable", NULL, NULL);
	//return;
	if(ret){
	    char buildString[100000];
	    buildString[0]='\0';
        HandleErrorRet(clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(char) * 100000, buildString, NULL));
        printf("build log:\n%s\n", buildString);
	}

	/* Create OpenCL Kernel */
	//HandleErrorPar(kernel = clCreateKernel(program, "crypt25", HANDLE_ERROR));
	
	kernel = cl_create_kernel(program, "crypt25");
	
	/* Gather information about host and device */
	uint32_t max_compute_units = 0;
	uint32_t max_work_group_size = 0;
	size_t *program_size = 0;
	
	HandleErrorRet(clGetProgramInfo ( program, CL_PROGRAM_BINARY_SIZES, sizeof(program_size), &program_size, NULL));
	HandleErrorRet(clGetDeviceInfo ( device_id, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(max_compute_units), &max_compute_units, NULL));
	HandleErrorRet(clGetDeviceInfo ( device_id, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(max_work_group_size), &max_work_group_size, NULL));

	/* Prepare test launch */
	uint32_t work_group_size = 128;
	uint32_t n = max_compute_units * work_group_size * 2;
	
	//host-side buffers for key related structures.
	cl_key_char *keys;
	uint32_t *keys_coalesced, *keys_sliced, *seeds, *sjis,
		keys_size = n*56*sizeof(uint32_t),
		seeds_size = n*sizeof(uint32_t),
		sjis_size = 256*256*2*sizeof(uint32_t),
		seeds_offset = keys_size,
		sjis_offset = seeds_offset+seeds_size;
	
	//host-size buffers for the structures used to transfer founds keys back to host.
	uint32_t *hit_bool, *hits,
		hit_bool_size = sizeof(uint32_t),
		hits_size = BATCH_SIZE*n*sizeof(uint32_t)*32, //This is where the bulk of the memory usage of this program comes from
		hit_bool_offset = sjis_offset+sjis_size,
		hits_offset = hit_bool_offset + hit_bool_size;
	
	//buffer allocated for keys will also contain three other parameters
	//because adding any more parameters past 6 causes a slowdown on my AMD 7850...
	//Haven't tested if that still happens on newer drivers, though. Must be an opencl
	//compiler quirk 
	uint32_t keys_and_extra_size = 
			keys_size
		+	seeds_size
		+	sjis_size
		+	hit_bool_size
		+	hits_size;
		
	//Allocate memory
	keys_coalesced = (uint32_t*)calloc(keys_and_extra_size, 1);
	seeds = (void *)keys_coalesced+seeds_offset;
	sjis = (void *)keys_coalesced+sjis_offset;
	hit_bool = (void *)keys_coalesced+hit_bool_offset;
	hits = (void *)keys_coalesced+hits_offset;

	assert(keys_coalesced != NULL /*too little memory*/);
	
	copy_sjis_tables(sjis);
	
	/* Allocate memory on gpu */
	keys_and_extra_gpu = cl_malloc(context, keys_and_extra_size);
    blocks_gpu = cl_malloc(context, n*256*sizeof(uint32_t));
	g_dictpool_gpu = cl_malloc(context, dictpool_size);
	wdk_gpu = cl_malloc(context, wdksize);
	wdw_gpu = cl_malloc(context, wdwsize);
	wa_gpu = cl_malloc(context, n*1024*sizeof(uint32_t));

    /* Write to memory on gpu */
	cl_copy_to(keys_and_extra_gpu, keys_coalesced, keys_and_extra_size, 0, command_queue);
	cl_copy_to(g_dictpool_gpu, dictpool, dictpool_size, 0, command_queue);
	cl_copy_to(wdk_gpu, wdk_pool, wdksize, 0, command_queue);
	cl_copy_to(wdw_gpu, wdw_pool, wdwsize, 0, command_queue);
	
    /* Set as arguments */
    HandleErrorRet(clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&keys_and_extra_gpu));
    HandleErrorRet(clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&blocks_gpu));
    HandleErrorRet(clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&wa_gpu));
	HandleErrorRet(clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&g_dictpool_gpu));
    HandleErrorRet(clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&wdk_gpu));
    HandleErrorRet(clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&wdw_gpu));
    

    /* Execute */
	int length = 25; //do a few batches with the same shared key to completely hide setup time.
	set_start_time();
	keys = calloc(8*32*n, sizeof(cl_key_char));
	keys_sliced = calloc(8*32*n, sizeof(uint32_t));
    for(a=0; a<length; a++){
		//printf("%d / %d\n", a, length);
		//Reset keys
        memset(keys_sliced,0,keys_size);
		memset(keys_coalesced,0,keys_size);
		
		//Generate pseudorandom shift-jis keys
		for(b=0; b<n*32; b++){
			keys[b*8+0] = shared_key[0];
			keys[b*8+1] = shared_key[1];
			keys[b*8+2] = shared_key[2];
			keys[b*8+5].type = 0;
			
			//Requiring the sixth character to be of type 1
			//makes setting keys on the gpu that much simpler
			while(keys[b*8+5].type != 1)cl_key_reset(keys+b*8,3);
			uint8_t key[8];
			for(c = 0; c < 8; c++)key[c] = keys[b*8+c].key;
			set_key_line(keys_sliced+((b/32)*56),key,b%32,6);	
		}
		
		coalesc_keys(keys_sliced,keys_coalesced,n);
		for(b = 0; b < n; b++)seeds[b] = rand();
		cl_copy_to(keys_and_extra_gpu, keys_coalesced, keys_size + seeds_size, 0, command_queue);
		
		for(b = 0; b <= 7; b++){
			for(c = 0; c <= BATCH_SIZE; c++){
				HandleErrorRet(clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &n, &work_group_size, 0, NULL, NULL));
				
			}
			
			HandleErrorRet(clEnqueueReadBuffer(command_queue, keys_and_extra_gpu, CL_TRUE, hit_bool_offset,
				hit_bool_size, hit_bool, 0, NULL, NULL));
			register_trips_generated(gpu, BATCH_SIZE*n*32);
			if(*hit_bool){
				*hit_bool = 0;
				//Copy 
				cl_copy_from(keys_and_extra_gpu, hits, hits_size, hits_offset, command_queue);

				//Examine hits
				for(c=0; c<n*BATCH_SIZE*32; c++){
					examine(hits[c], keys+(8*(c/BATCH_SIZE)));
					hits[c] = 0;
				}
				
				//Reset
				cl_copy_to(keys_and_extra_gpu, hit_bool, hit_bool_size+hits_size, hit_bool_offset, command_queue);

			}
		}
		
    }
	

    /* Finalization */
    HandleErrorRet(clFlush(command_queue));
    HandleErrorRet(clFinish(command_queue));
    HandleErrorRet(clReleaseMemObject(keys_and_extra_gpu));
    HandleErrorRet(clReleaseMemObject(blocks_gpu));
    HandleErrorRet(clReleaseMemObject(g_dictpool_gpu));
    HandleErrorRet(clReleaseMemObject(wdk_gpu));
    HandleErrorRet(clReleaseMemObject(wdw_gpu));
    HandleErrorRet(clReleaseMemObject(wa_gpu));
    HandleErrorRet(clReleaseKernel(kernel));
    HandleErrorRet(clReleaseProgram(program));
    HandleErrorRet(clReleaseCommandQueue(command_queue));
    HandleErrorRet(clReleaseContext(context));
	free(keys);
	free(keys_coalesced);
    return 0;
}

void cl_configure_b(char *regs, char *cl){
	//Should probably make some benchmark to determine how many registers to use...
	int regcount = 64;
	int i = 0;
	for(; i < 64; i++){
		regs[i] = i < regcount;
		if(regs[i]){
			cl += sprintf(cl, "#define b%d b(%d)\n",i,i);
		}
		else{
			cl += sprintf(cl, "#define b%d lb(%d)\n",i,i-regcount);
		}
	}
	
	cipher_registers = regcount;
}

int gpu_init(uint32_t seed){
    srand(time(NULL));
	cl_configure_b(B_reg, B_cl);
	cl_key_init(seed);
	wdict_setup_gpu(&dictpool, &wdk_pool, &wdw_pool,
					&dictpool_size, &wdksize, &wdwsize,
					&min_dictpool, &n_dictpool);
					
	source_crypt = (char *)readFile("./gpu.cl",&len_crypt);
	source_sboxdef = (char *)readFile("./sboxdef.cl", &len_sboxdef);
	source_cmp = (char *)readFile("./cmp.cl", &len_cmp);
	source_des = (char *)readFile("./des.cl", &len_des);
	assert(source_crypt!=NULL /* reading gpu source code failed */);
	assert(len_crypt>0 /* gpu source was 0 characters long */);
	assert(source_sboxdef!=NULL /* reading gpu source code failed */);
	assert(len_sboxdef>0 /* gpu source was 0 characters long */);
	assert(source_cmp!=NULL /* reading cmp source code failed */);
	assert(len_cmp>0 /* cmp source was 0 characters long */);
	assert(source_des!=NULL /* reading des source code failed */);
	assert(len_des>0 /* des source was 0 characters long */);
					
	return cl_get_num_gpus();
}

void *gpu_main(void *dummyarg /* Takes no argument */){
	int gpu = register_gpu();
	printf("dk size: %i64u\n",wdksize);
    while(1){
		do_search(gpu);
		
	}
    return NULL;
}