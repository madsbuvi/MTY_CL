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
#include <pthread.h>
#include "desconst.h"
#include "mty_cl.h"
#include "cl_util.h"
#include "des_std.h"

#if defined(WIN32)

#include <windows.h>

#elif defined(__GNUC__)

#include <sys/time.h>
#endif

#include "wdict.h"
#include "log.h"
#include "crypt3.h"
#include "key_cl.h"

//Structures used by MTY for matching tripcodes
void *dictpool,*wdk_pool,*wdw_pool;
int32_t dictpool_size, wdksize, wdwsize, min_dictpool, n_dictpool;

//Sources
char *source_crypt, *source_sboxdef, *source_cmp,  *source_des, *source_transpose;
size_t len_crypt, len_sboxdef, len_cmp, len_wdict_config, len_des, len_transpose;

//List of all possible two last characters
uint32_t *end_keys, num_end_keys;

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
	uint32_t a,b;
	for(a=6,b=0;b<length;b++){
		mask = 1<<6;
		key[a]=(plaintext[b]&mask)>0?-1:0;a--;mask>>=1;
		key[a]=(plaintext[b]&mask)>0?-1:0;a--;mask>>=1;
		key[a]=(plaintext[b]&mask)>0?-1:0;a--;mask>>=1;
		key[a]=(plaintext[b]&mask)>0?-1:0;a--;mask>>=1;
		key[a]=(plaintext[b]&mask)>0?-1:0;a--;mask>>=1;
		key[a]=(plaintext[b]&mask)>0?-1:0;a--;mask>>=1;
		key[a]=(plaintext[b]&mask)>0?-1:0;a--;mask>>=1;
		a+=14;
	}
}

//Sets a key to the given slice
//Presumes that the slice is already cleared
void set_key_line(uint32_t *key, uint8_t *plaintext, uint32_t line, int32_t length){
	uint32_t mask;
	uint32_t a,b;
	line = 1u << line;
	for(a=6,b=0;b<length;b++){
		mask = 1<<6;
		key[a]|=line&((plaintext[b]&mask)>0?-1:0);a--;mask>>=1;
		key[a]|=line&((plaintext[b]&mask)>0?-1:0);a--;mask>>=1;
		key[a]|=line&((plaintext[b]&mask)>0?-1:0);a--;mask>>=1;
		key[a]|=line&((plaintext[b]&mask)>0?-1:0);a--;mask>>=1;
		key[a]|=line&((plaintext[b]&mask)>0?-1:0);a--;mask>>=1;
		key[a]|=line&((plaintext[b]&mask)>0?-1:0);a--;mask>>=1;
		key[a]|=line&((plaintext[b]&mask)>0?-1:0);a--;mask>>=1;
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

void examine(uint32_t hits, cl_key_char *keys, int item){
	if(!hits)return;
	uint8_t key[9];
	int i;
	for(i = 0; i < 6; i++)key[i] = keys[i].key;
	char salt[2] = {salt_chars[key[1]],salt_chars[key[2]]};
	key[6] = hits&0xff;
	key[7] = hits/0x100;
	key[8] = cl_key_get_last(key[6], key[7]);
	cl_key_make_safe(key);
	char *hash = crypt((char *)key, salt);
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

//How many searches to do before checking for a hit
//Checking hits requires waiting for a read and therefore chips into getting full usage of the gpu,
//so i don't do that every time.
#define BATCH_SIZE 100

/*
 * Infinitely searches random Japanese Shift-JIS keys
 */
int do_search(int gpu){
	cl_key_char shared_key[9];
    cl_device_id device_id = NULL;
	cl_context context = NULL;
	cl_command_queue command_queue = NULL;
	cl_mem keys_and_extra_gpu, key_ends_gpu, wa_gpu, g_dictpool_gpu,
	wdk_gpu, wdw_gpu;
	cl_program program = NULL;
	cl_kernel kernel = NULL, inc_kernel = NULL, clear_kernel = NULL;
	cl_int ret;
	uint8_t salt[2], *possible_keys, *used_keys;
	uint32_t number_of_possible_keys;
	int32_t a,b,c;
	size_t work_group_size;
	size_t n;
	
	//Will contain a list of defines to pass to the opencl compiler
	//Instead of having another level of indirection with the extension matrix, i'll just
	//hardcode it and recompile when it needs to change.
	//So long as recompilation is rare enough, this is greatly rewarding.
	char E_cl[2048];
	
	//Will contain defines on the bits of a shared key
	char K_cl[2048*2];
	
	//Will contain a few configurations related to wdict
	char wdict_config[1024];
	
	//Set up the shared key
	//The first 3 characters of each key beecomes a set of #defined constants in the 
	//generated source code.
	cl_key_random(shared_key, 0);
	salt[0] = salt_chars[shared_key[1].key];
	salt[1] = salt_chars[shared_key[2].key];
	cl_set_salt(salt, E_cl);
	cl_setup_shared_key(shared_key, K_cl);
	
	//the type and value of the third shared key limits the possible values
	//of the following keys. I deal with this by generating ALL the possible keys
	//and then to avoid any duplicate keys i simply pick a not previously used
	//entry from this list.
	uint32_t backup = shared_key[2].key;
	number_of_possible_keys = generate_all(shared_key, &possible_keys);
	used_keys = (uint8_t *)calloc(number_of_possible_keys, sizeof(uint8_t));
	shared_key[2].key = backup;
	
	len_wdict_config = sprintf(wdict_config,
	#if NVIDIA
	
	#else
			"#define AMD 1\n"
	#endif
			"#define MIN_DICTPOOL %d\n"
			"#define N_DICTPOOL %d\n"
			"#define BATCH_SIZE %d\n",
			min_dictpool, n_dictpool, BATCH_SIZE);

	/* Get Platform and Device Info */
	init_cl_gpu_specific(gpu, &device_id, &context, & command_queue);

	/* Create Kernel Program from the source */
	const char *sources[] = {
		K_cl,
		E_cl,
		#if NVIDIA
		source_transpose,
		#else
		
		#endif
		source_des,
		wdict_config,
		source_sboxdef,
		source_cmp,
		source_crypt,
		};
	const size_t lengths[] = {
		strlen(K_cl),
		strlen(E_cl),
		#if NVIDIA
		len_transpose,
		#else
		
		#endif
		len_des,
		len_wdict_config,
		len_sboxdef,
		len_cmp,
		len_crypt
		};
	
	fprintf(stderr, "Compiling... ");
	HandleErrorPar(program = clCreateProgramWithSource(context, sizeof(lengths)/sizeof(const size_t), sources,
	lengths, HANDLE_ERROR));
	
	/* Build Kernel Program */
	#if NVIDIA
	ret = clBuildProgram(program, 1, &device_id, "", NULL, NULL);
	#else
	ret = clBuildProgram(program, 1, &device_id, "-cl-opt-disable", NULL, NULL);
	#endif
	if(ret){
		fprintf(stderr, "Error code %d:\n", ret);
		size_t ssiz = 0;
        HandleErrorRet(clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &ssiz));
	    char *buildString = malloc(ssiz+1);
	    buildString[0]='\0';
        HandleErrorRet(clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, ssiz, buildString, NULL));
        printf("build log:\n%s\n", buildString);
		free(buildString);
	}
	fprintf(stderr, "Done.\n");

	/* Create OpenCL Kernel */
	
	kernel = cl_create_kernel(program, "crypt25");
	inc_kernel = cl_create_kernel(program, "inc_key_offset");
	clear_kernel = cl_create_kernel(program, "clear_hits");
	
	/* Gather information about host and device */
	uint32_t max_compute_units = 0;
	size_t max_work_group_size = 0;
	uint64_t device_lmem_size = 0;
	
	size_t max_kernel_group_size = 0;
	uint64_t kernel_lmem_size = 0;
	size_t kernel_pref_size = 0;
	uint64_t kernel_priv_size = 0;
	size_t *program_size = 0;
	
	HandleErrorRet(clGetProgramInfo ( program, CL_PROGRAM_BINARY_SIZES, sizeof(program_size), &program_size, NULL));
	HandleErrorRet(clGetDeviceInfo ( device_id, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(max_compute_units), &max_compute_units, NULL));
	HandleErrorRet(clGetDeviceInfo ( device_id, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(max_work_group_size), &max_work_group_size, NULL));
	HandleErrorRet(clGetDeviceInfo ( device_id, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(device_lmem_size), &device_lmem_size, NULL));
	HandleErrorRet(clGetKernelWorkGroupInfo  ( kernel, device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(max_kernel_group_size), &max_kernel_group_size, NULL));
	HandleErrorRet(clGetKernelWorkGroupInfo  ( kernel, device_id, CL_KERNEL_LOCAL_MEM_SIZE, sizeof(kernel_lmem_size), &kernel_lmem_size, NULL));
	HandleErrorRet(clGetKernelWorkGroupInfo  ( kernel, device_id, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, sizeof(kernel_pref_size), &kernel_pref_size, NULL));
	HandleErrorRet(clGetKernelWorkGroupInfo  ( kernel, device_id, CL_KERNEL_PRIVATE_MEM_SIZE, sizeof(kernel_priv_size), &kernel_priv_size, NULL));

	#if NVIDIA
	work_group_size = max_kernel_group_size;
	n = max_compute_units * work_group_size;
	#else
	work_group_size = 128;
	n = max_compute_units * work_group_size * 2;
	#endif
	/* Prepare test launch */
	
	//host-side buffers for key related structures.
	cl_key_char *keys;
	uint32_t *keys_coalesced, *keys_sliced,
		keys_size = n*56*sizeof(uint32_t),
		key_end_index_size = sizeof(uint32_t),
		key_end_index_offset = keys_size;
		
	
	//host-size buffers for the structures used to transfer founds keys back to host.
	uint32_t *hit_bool,
		hit_bool_size = sizeof(uint32_t),
		hits_size = BATCH_SIZE*n*sizeof(uint32_t)*32,
		hit_bool_offset = key_end_index_offset+key_end_index_size,
		hits_offset = hit_bool_offset + hit_bool_size;
	
	//buffer allocated for keys will also contain three other parameters
	//because adding any more parameters past 6 causes a slowdown on my AMD 7850...
	//Haven't tested if that still happens on newer drivers, though. Must be an opencl
	//compiler quirk?
	uint32_t keys_and_extra_size = 
			keys_size
		+	key_end_index_size
		+	hit_bool_size;
		
	//Allocate memory
	keys_coalesced = (uint32_t*)calloc(keys_and_extra_size, 1);
	hit_bool = (void *)keys_coalesced+hit_bool_offset;

	assert(keys_coalesced != NULL /*too little memory*/);
	
	
	/* Allocate memory on gpu */
	keys_and_extra_gpu = cl_malloc(context, keys_and_extra_size + hits_size);
    key_ends_gpu = cl_malloc(context, 128*128*2*sizeof(uint32_t) + 32*64*n*sizeof(uint32_t));
	g_dictpool_gpu = cl_malloc(context, dictpool_size*2);
	wdk_gpu = cl_malloc(context, wdksize*2);
	wdw_gpu = cl_malloc(context, wdwsize*2);
	wa_gpu = cl_malloc(context, n*1024*sizeof(uint32_t));

    /* Write to memory on gpu */
	cl_copy_to(keys_and_extra_gpu, keys_coalesced, keys_and_extra_size, 0, command_queue);
	cl_copy_to(g_dictpool_gpu, dictpool, dictpool_size, 0, command_queue);
	cl_copy_to(wdk_gpu, wdk_pool, wdksize, 0, command_queue);
	cl_copy_to(wdw_gpu, wdw_pool, wdwsize, 0, command_queue);
	cl_copy_to(key_ends_gpu, end_keys, num_end_keys*2*sizeof(uint32_t), 0, command_queue);
	
    /* Set as arguments */
    HandleErrorRet(clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&keys_and_extra_gpu));
    HandleErrorRet(clSetKernelArg(inc_kernel, 0, sizeof(cl_mem), (void *)&keys_and_extra_gpu));
    HandleErrorRet(clSetKernelArg(clear_kernel, 0, sizeof(cl_mem), (void *)&keys_and_extra_gpu));
    HandleErrorRet(clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&key_ends_gpu));
    HandleErrorRet(clSetKernelArg(clear_kernel, 1, sizeof(uint32_t), (void *)&hits_size));
    HandleErrorRet(clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&wa_gpu));
	HandleErrorRet(clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&g_dictpool_gpu));
    HandleErrorRet(clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&wdk_gpu));
    HandleErrorRet(clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&wdw_gpu));
    

    /* Execute */
	int length = number_of_possible_keys / (n*32);
	uint32_t prng_state = usec();
	uint32_t selector = prng_state = next_int(prng_state);
	selector %= number_of_possible_keys;
	
	set_start_time();
	keys = calloc(8*32*n, sizeof(cl_key_char));
	keys_sliced = calloc(8*32*n, sizeof(uint32_t));
				HandleErrorRet(clEnqueueNDRangeKernel(command_queue, clear_kernel, 1, NULL, &n, &work_group_size, 0, NULL, NULL));
    for(a=0; a<length; a++){
		//Reset keys
        memset(keys_sliced,0,keys_size);
		memset(keys_coalesced,0,keys_size);
		
		//Select a shift-jis key
		for(b=0; b<n*32; b++){
			int i = selector % number_of_possible_keys;
			keys[b*8+0] = shared_key[0];
			keys[b*8+1] = shared_key[1];
			keys[b*8+2] = shared_key[2];
			keys[b*8+3].key = possible_keys[3*i+0];
			keys[b*8+4].key = possible_keys[3*i+1];
			keys[b*8+5].key = possible_keys[3*i+2];
			
			selector++;
			
			uint8_t key[8];
			for(c = 0; c < 6; c++)key[c] = keys[b*8+c].key;
			set_key_line(keys_sliced+((b/32)*56),key,b%32,6);	
		}
		
		coalesc_keys(keys_sliced,keys_coalesced,n);
		cl_copy_to(keys_and_extra_gpu, keys_coalesced, keys_size + key_end_index_size, 0, command_queue);
		
		for(b = 0; b <= (num_end_keys/BATCH_SIZE); b++){
			
			int final_iteration = b == (num_end_keys/BATCH_SIZE);
			for(c = 0; c <= (final_iteration ? (num_end_keys%BATCH_SIZE) : BATCH_SIZE); c++){
				HandleErrorRet(clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &n, &work_group_size, 0, NULL, NULL));
				HandleErrorRet(clEnqueueNDRangeKernel(command_queue, inc_kernel, 1, NULL, &n, &work_group_size, 0, NULL, NULL));
				
			}
			
			HandleErrorRet(clEnqueueReadBuffer(command_queue, keys_and_extra_gpu, CL_TRUE, hit_bool_offset,
				hit_bool_size, hit_bool, 0, NULL, NULL));
			register_trips_generated(gpu, BATCH_SIZE*n*32);
			if(*hit_bool){
				*hit_bool = 0;

				uint32_t work[32*n];
				uint32_t i,j;
				for(i = 0; i < BATCH_SIZE; i++){
					cl_copy_from(keys_and_extra_gpu, work, sizeof(work), hits_offset+sizeof(work)*i, command_queue);
					for(j = 0; j < 32*n; j++){
						uint32_t working_key = (i*32*n + j)/BATCH_SIZE;
						examine(work[j], keys + 8*working_key, working_key);
					}
				}
				
				HandleErrorRet(clEnqueueNDRangeKernel(command_queue, clear_kernel, 1, NULL, &n, &work_group_size, 0, NULL, NULL));

			}
		}
		
    }
	

    /* Finalization */
    HandleErrorRet(clFlush(command_queue));
    HandleErrorRet(clFinish(command_queue));
    HandleErrorRet(clReleaseMemObject(keys_and_extra_gpu));
    HandleErrorRet(clReleaseMemObject(key_ends_gpu));
    HandleErrorRet(clReleaseMemObject(g_dictpool_gpu));
    HandleErrorRet(clReleaseMemObject(wdk_gpu));
    HandleErrorRet(clReleaseMemObject(wdw_gpu));
    HandleErrorRet(clReleaseMemObject(wa_gpu));
    HandleErrorRet(clReleaseKernel(kernel));
    HandleErrorRet(clReleaseProgram(program));
    HandleErrorRet(clReleaseCommandQueue(command_queue));
    HandleErrorRet(clReleaseContext(context));
	free(keys);
	free(keys_sliced);
	free(keys_coalesced);
	free(used_keys);
	free(possible_keys);
    return 0;
}

int gpu_init(uint32_t seed){
    srand(time(NULL));
	cl_key_init(seed);
	wdict_setup_gpu(&dictpool, &wdk_pool, &wdw_pool,
					&dictpool_size, &wdksize, &wdwsize,
					&min_dictpool, &n_dictpool);
					
	source_crypt = (char *)readFile("./gpu.cl",&len_crypt);
	source_sboxdef = (char *)readFile("./sboxdef.cl", &len_sboxdef);
	source_cmp = (char *)readFile("./cmp.cl", &len_cmp);
	#if NVIDIA
	source_des = (char *)readFile("./DES.cl", &len_des);
	source_transpose = (char *)readFile("./transpose.cl", &len_transpose);
	assert(source_transpose!=NULL /* reading des source code failed */);
	assert(len_transpose>0 /* des source was 0 characters long */);
	#else
	source_des = (char *)readFile("./DES_AMD.cl", &len_des);
	#endif
	assert(source_crypt!=NULL /* reading gpu source code failed */);
	assert(len_crypt>0 /* gpu source was 0 characters long */);
	assert(source_sboxdef!=NULL /* reading gpu source code failed */);
	assert(len_sboxdef>0 /* gpu source was 0 characters long */);
	assert(source_cmp!=NULL /* reading cmp source code failed */);
	assert(len_cmp>0 /* cmp source was 0 characters long */);
	assert(source_des!=NULL /* reading des source code failed */);
	assert(len_des>0 /* des source was 0 characters long */);
	num_end_keys = generate_all_end(&end_keys);
	assert(num_end_keys>0);
	assert(end_keys!=NULL);
	return cl_get_num_gpus();
}

void *gpu_main(void *dummyarg /* Takes no argument */){
	int gpu = register_gpu();
    pthread_t thId = pthread_self();
    pthread_attr_t thAttr;
    int policy = 0;
    int max_prio_for_policy = 0;
    int ret = 0;

    ret = pthread_attr_init(&thAttr);if(ret){fprintf(stderr, "Could not init\n");}
    ret = pthread_attr_getschedpolicy(&thAttr, &policy);if(ret){fprintf(stderr, "Could not get\n");}
    max_prio_for_policy = sched_get_priority_max(policy);


    ret = pthread_setschedprio(thId, max_prio_for_policy);if(ret){fprintf(stderr, "Could not set\n");}
    ret = pthread_attr_destroy(&thAttr);if(ret){fprintf(stderr, "Could not destroy\n");}
    while(1){
		do_search(gpu);
	}
    return NULL;
}
