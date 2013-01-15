/*
 * file: cl_util.h
 * 
 * Several utility functions for OpenCL
 *
 * Written by Mads Buvik Sandvei (https://github.com/madsbuvi/, madsbuvi@stud.ntnu.no)
 */
 #include <stdint.h>
#include <CL/cl.h>

/*
 * Performs a binary read of the given file, saving the pointer to the buffer in source and
 * the amount of bytes read in len.
 * Part of cl util because when using opencl you frequently need to read in whole source files
 * and keep them in a buffer.
 */
int8_t *readFile(const char *filename, size_t *len);

/*
 *  Function to get an error string based on error code
 *  Copied from enj @ enja.org
 */
const char* oclErrorString(cl_int error);

/*
 *  Function to "handle" an error code.
 */
void handle_error(cl_int error, int32_t line, const  char *file);

//Defines to handle errors:
//You do not need to use semicolons after these defines, but it's a good habit.

/*
 *  Define to wrap around a function that returns error code
 *  E.g. HandleErrorRet(clGetPlatformIDs(1, &platform_id, &ret_num_platforms));
 */
#define HandleErrorRet(clfunc)\
{\
    cl_int error = (clfunc);\
    if(error){\
        handle_error(error, __LINE__, __FILE__);\
        exit(-1);\
    }\
}

/*
 *  Define to wrap around a function that returns error code to a parameter
 *  Should also wrap around the assignment of the result.
 *
 *  MAKE SURE YOU PASS "HANDLE_ERROR" AS THE ERROR RETURN PARAMETER
 *
 *  E.G. HandleErrorPar(context = clCreateContext(NULL, 1, &device_id, NULL, NULL, HANDLE_ERROR));
 */
#define HANDLE_ERROR &error
#define HandleErrorPar(clfunc)\
{\
    cl_int error;\
    (clfunc);\
    if(error){\
        handle_error(error, __LINE__, __FILE__);\
        exit(-1);\
    }\
}


/*
 * Function that examines whether the host is little or big endian
 * Returns 1 if little endian, 0 if big endian.
 */
int little_endian();


/*
 * Define to wrap around the convoluted method of getting a truly random value from 0 to interval from the rand() function
 */
#define random(interval) ((int)((double)rand() / ((double)RAND_MAX + 1) * interval))

/*
 * Function to generate a random ASCII string of length len. Does not allocate its own memory.
 */
void random_string(uint8_t *string, uint64_t len);


/*
 * printout defines for ease of debugging
 */
#define prints(a) printf("%s\n", a );
#define printi(a) printf(#a ": %d\n",a);
#define printc(a) printf(#a ": %c\n",a);
#define printu(a) printf(#a ": %u\n",a);
#define printl(a) printf(#a ": %I64\n",a);
#define printul(a) printf(#a ": %I64u\n",a);

/*
 * Initializes a single context for GPU.
 * Handles all errors by my standards itself, no need to return anything.
 */
 //Creates a plain context and command queue.
 void init_cl_gpu_single(cl_device_id *device_id, cl_context *context, cl_command_queue *command_queue);
 
 //Creates a plain context and command queue for the specific device number.
 //(Does not use device_id as input, but output. gpu_id is the input. If there are n>gpu_id devices, it will use the nth
 // gpu it finds when going through them and their attached platform). Useful only when you want to separately initialize
 // and use each GPU and you don't care about the detais of which gpu gets what work.
 int init_cl_gpu_specific(int32_t gpu_id, cl_device_id *device_id, cl_context *context, cl_command_queue *command_queue);
 
 //Creates a plain context with a profiling-enabled command queue.
 void init_cl_gpu_single_profiling(cl_device_id *device_id, cl_context *context, cl_command_queue *command_queue);
 
 
 /*
  * Builds the variable length list of sources as a concatenated source.
  */
 #define cl_build_all(context, device_id, program_ptr, ...)\
 {\
	char *sources[] = {__VA_ARGS__};\
	int n_sources = sizeof(sources)/sizeof(char *);\
	cl_build(sources, n_sources, context, device_id, program_ptr);\
}
void cl_build(char **sources, int n_sources, cl_context context, cl_device_id device_id, cl_program *program);

/*
 * Creates an OpenCL kernel
 */
 #define cl_create_kernel(program, kernelname) __cl_create_kernel__(program, kernelname, __LINE__, __FILE__)
 cl_kernel __cl_create_kernel__(cl_program program, const char *kernelname, int line, const char *file);

/*
 * Good old malloc.
 */
#define cl_malloc(context, size) __cl_malloc__(context, size, __LINE__, __FILE__)
cl_mem __cl_malloc__(cl_context context, size_t size, int line, const  char *file);

/*
 * Memcpy
 */
#define cl_copy_to(to, from, size, offset, queue) __cl_copy_to__(to, from, size, offset, queue, __LINE__, __FILE__)
void __cl_copy_to__(cl_mem to, void *from, size_t size, size_t offset, cl_command_queue command_queue, int line, const char *file);

#define cl_copy_from(from, to, size, offset, queue) __cl_copy_from__(from, to, size, offset, queue, __LINE__, __FILE__)
void __cl_copy_from__(cl_mem from, void *to, size_t size, size_t offset, cl_command_queue command_queue, int line, const char *file);

/*
 * Device info functions
 */
#define cl_get_info(devid, info) __cl_get_info__(devid, info, __LINE__, __FILE__)
size_t __cl_get_info__(cl_device_id device_id, cl_device_info info, int line, const char *file);
uint32_t cl_get_num_gpus();
/*
 * opencl function call macros
 * Upto 8 arguments currently. easily extended.
 * Dimsize and dimwgsize must be pointers.
 */
#define cl_call_1(kernel, command_queue, dimsize, dimwgsize, ndims, arg)\
HandleErrorRet(clSetKernelArg(kernel, 0, sizeof(arg), (void *)&arg));\
HandleErrorRet(clEnqueueNDRangeKernel(command_queue, kernel, ndims, NULL, dimsize, dimwgsize, 0, NULL, NULL));

#define cl_call_2(kernel, command_queue, dimsize, dimwgsize, ndims, arg0, arg1)\
HandleErrorRet(clSetKernelArg(kernel, 0, sizeof(arg0), (void *)&arg0));\
HandleErrorRet(clSetKernelArg(kernel, 1, sizeof(arg1), (void *)&arg1));\
HandleErrorRet(clEnqueueNDRangeKernel(command_queue, kernel, ndims, NULL, dimsize, dimwgsize, 0, NULL, NULL));

#define cl_call_3(kernel, command_queue, dimsize, dimwgsize, ndims, arg0, arg1, arg2)\
HandleErrorRet(clSetKernelArg(kernel, 0, sizeof(arg0), (void *)&arg0));\
HandleErrorRet(clSetKernelArg(kernel, 1, sizeof(arg1), (void *)&arg1));\
HandleErrorRet(clSetKernelArg(kernel, 2, sizeof(arg2), (void *)&arg2));\
HandleErrorRet(clEnqueueNDRangeKernel(command_queue, kernel, ndims, NULL, dimsize, dimwgsize, 0, NULL, NULL));

#define cl_call_4(kernel, command_queue, dimsize, dimwgsize, ndims, arg0, arg1, arg2, arg3)\
HandleErrorRet(clSetKernelArg(kernel, 0, sizeof(arg0), (void *)&arg0));\
HandleErrorRet(clSetKernelArg(kernel, 1, sizeof(arg1), (void *)&arg1));\
HandleErrorRet(clSetKernelArg(kernel, 2, sizeof(arg2), (void *)&arg2));\
HandleErrorRet(clSetKernelArg(kernel, 3, sizeof(arg3), (void *)&arg3));\
HandleErrorRet(clEnqueueNDRangeKernel(command_queue, kernel, ndims, NULL, dimsize, dimwgsize, 0, NULL, NULL));

#define cl_call_5(kernel, command_queue, dimsize, dimwgsize, ndims, arg0, arg1, arg2, arg3, arg4)\
HandleErrorRet(clSetKernelArg(kernel, 0, sizeof(arg0), (void *)&arg0));\
HandleErrorRet(clSetKernelArg(kernel, 1, sizeof(arg1), (void *)&arg1));\
HandleErrorRet(clSetKernelArg(kernel, 2, sizeof(arg2), (void *)&arg2));\
HandleErrorRet(clSetKernelArg(kernel, 3, sizeof(arg3), (void *)&arg3));\
HandleErrorRet(clSetKernelArg(kernel, 4, sizeof(arg4), (void *)&arg4));\
HandleErrorRet(clEnqueueNDRangeKernel(command_queue, kernel, ndims, NULL, dimsize, dimwgsize, 0, NULL, NULL));

#define cl_call_6(kernel, command_queue, dimsize, dimwgsize, ndims, arg0, arg1, arg2, arg3, arg4, arg5)\
HandleErrorRet(clSetKernelArg(kernel, 0, sizeof(arg0), (void *)&arg0));\
HandleErrorRet(clSetKernelArg(kernel, 1, sizeof(arg1), (void *)&arg1));\
HandleErrorRet(clSetKernelArg(kernel, 2, sizeof(arg2), (void *)&arg2));\
HandleErrorRet(clSetKernelArg(kernel, 3, sizeof(arg3), (void *)&arg3));\
HandleErrorRet(clSetKernelArg(kernel, 4, sizeof(arg4), (void *)&arg4));\
HandleErrorRet(clSetKernelArg(kernel, 5, sizeof(arg5), (void *)&arg5));\
HandleErrorRet(clEnqueueNDRangeKernel(command_queue, kernel, ndims, NULL, dimsize, dimwgsize, 0, NULL, NULL));

#define cl_call_7(kernel, command_queue, dimsize, dimwgsize, ndims, arg0, arg1, arg2, arg3, arg4, arg5, arg6)\
HandleErrorRet(clSetKernelArg(kernel, 0, sizeof(arg0), (void *)&arg0));\
HandleErrorRet(clSetKernelArg(kernel, 1, sizeof(arg1), (void *)&arg1));\
HandleErrorRet(clSetKernelArg(kernel, 2, sizeof(arg2), (void *)&arg2));\
HandleErrorRet(clSetKernelArg(kernel, 3, sizeof(arg3), (void *)&arg3));\
HandleErrorRet(clSetKernelArg(kernel, 4, sizeof(arg4), (void *)&arg4));\
HandleErrorRet(clSetKernelArg(kernel, 5, sizeof(arg5), (void *)&arg5));\
HandleErrorRet(clSetKernelArg(kernel, 6, sizeof(arg6), (void *)&arg6));\
HandleErrorRet(clEnqueueNDRangeKernel(command_queue, kernel, ndims, NULL, dimsize, dimwgsize, 0, NULL, NULL));

#define cl_call_8(kernel, command_queue, dimsize, dimwgsize, ndims, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7)\
HandleErrorRet(clSetKernelArg(kernel, 0, sizeof(arg0), (void *)&arg0));\
HandleErrorRet(clSetKernelArg(kernel, 1, sizeof(arg1), (void *)&arg1));\
HandleErrorRet(clSetKernelArg(kernel, 2, sizeof(arg2), (void *)&arg2));\
HandleErrorRet(clSetKernelArg(kernel, 3, sizeof(arg3), (void *)&arg3));\
HandleErrorRet(clSetKernelArg(kernel, 4, sizeof(arg4), (void *)&arg4));\
HandleErrorRet(clSetKernelArg(kernel, 5, sizeof(arg5), (void *)&arg5));\
HandleErrorRet(clSetKernelArg(kernel, 6, sizeof(arg6), (void *)&arg6));\
HandleErrorRet(clSetKernelArg(kernel, 7, sizeof(arg7), (void *)&arg7));\
HandleErrorRet(clEnqueueNDRangeKernel(command_queue, kernel, ndims, NULL, dimsize, dimwgsize, 0, NULL, NULL));


#define cl_call_profiling_1(kernel, command_queue, dimsize, dimwgsize, ndims, event, arg)\
HandleErrorRet(clSetKernelArg(kernel, 0, sizeof(arg), (void *)&arg));\
HandleErrorRet(clEnqueueNDRangeKernel(command_queue, kernel, ndims, NULL, dimsize, dimwgsize, 0, NULL, event));

#define cl_call_profiling_2(kernel, command_queue, dimsize, dimwgsize, ndims, event, arg0, arg1)\
HandleErrorRet(clSetKernelArg(kernel, 0, sizeof(arg0), (void *)&arg0));\
HandleErrorRet(clSetKernelArg(kernel, 1, sizeof(arg1), (void *)&arg1));\
HandleErrorRet(clEnqueueNDRangeKernel(command_queue, kernel, ndims, NULL, dimsize, dimwgsize, 0, NULL, event));

#define cl_call_profiling_3(kernel, command_queue, dimsize, dimwgsize, ndims, event, arg0, arg1, arg2)\
HandleErrorRet(clSetKernelArg(kernel, 0, sizeof(arg0), (void *)&arg0));\
HandleErrorRet(clSetKernelArg(kernel, 1, sizeof(arg1), (void *)&arg1));\
HandleErrorRet(clSetKernelArg(kernel, 2, sizeof(arg2), (void *)&arg2));\
HandleErrorRet(clEnqueueNDRangeKernel(command_queue, kernel, ndims, NULL, dimsize, dimwgsize, 0, NULL, event));

#define cl_call_profiling_4(kernel, command_queue, dimsize, dimwgsize, ndims, event, arg0, arg1, arg2, arg3)\
HandleErrorRet(clSetKernelArg(kernel, 0, sizeof(arg0), (void *)&arg0));\
HandleErrorRet(clSetKernelArg(kernel, 1, sizeof(arg1), (void *)&arg1));\
HandleErrorRet(clSetKernelArg(kernel, 2, sizeof(arg2), (void *)&arg2));\
HandleErrorRet(clSetKernelArg(kernel, 3, sizeof(arg3), (void *)&arg3));\
HandleErrorRet(clEnqueueNDRangeKernel(command_queue, kernel, ndims, NULL, dimsize, dimwgsize, 0, NULL, event));

#define cl_call_profiling_5(kernel, command_queue, dimsize, dimwgsize, ndims, event, arg0, arg1, arg2, arg3, arg4)\
HandleErrorRet(clSetKernelArg(kernel, 0, sizeof(arg0), (void *)&arg0));\
HandleErrorRet(clSetKernelArg(kernel, 1, sizeof(arg1), (void *)&arg1));\
HandleErrorRet(clSetKernelArg(kernel, 2, sizeof(arg2), (void *)&arg2));\
HandleErrorRet(clSetKernelArg(kernel, 3, sizeof(arg3), (void *)&arg3));\
HandleErrorRet(clSetKernelArg(kernel, 4, sizeof(arg4), (void *)&arg4));\
HandleErrorRet(clEnqueueNDRangeKernel(command_queue, kernel, ndims, NULL, dimsize, dimwgsize, 0, NULL, event));

#define cl_call_profiling_6(kernel, command_queue, dimsize, dimwgsize, ndims, event, arg0, arg1, arg2, arg3, arg4, arg5)\
HandleErrorRet(clSetKernelArg(kernel, 0, sizeof(arg0), (void *)&arg0));\
HandleErrorRet(clSetKernelArg(kernel, 1, sizeof(arg1), (void *)&arg1));\
HandleErrorRet(clSetKernelArg(kernel, 2, sizeof(arg2), (void *)&arg2));\
HandleErrorRet(clSetKernelArg(kernel, 3, sizeof(arg3), (void *)&arg3));\
HandleErrorRet(clSetKernelArg(kernel, 4, sizeof(arg4), (void *)&arg4));\
HandleErrorRet(clSetKernelArg(kernel, 5, sizeof(arg5), (void *)&arg5));\
HandleErrorRet(clEnqueueNDRangeKernel(command_queue, kernel, ndims, NULL, dimsize, dimwgsize, 0, NULL, event));

#define cl_call_profiling_7(kernel, command_queue, dimsize, dimwgsize, ndims, event, arg0, arg1, arg2, arg3, arg4, arg5, arg6)\
HandleErrorRet(clSetKernelArg(kernel, 0, sizeof(arg0), (void *)&arg0));\
HandleErrorRet(clSetKernelArg(kernel, 1, sizeof(arg1), (void *)&arg1));\
HandleErrorRet(clSetKernelArg(kernel, 2, sizeof(arg2), (void *)&arg2));\
HandleErrorRet(clSetKernelArg(kernel, 3, sizeof(arg3), (void *)&arg3));\
HandleErrorRet(clSetKernelArg(kernel, 4, sizeof(arg4), (void *)&arg4));\
HandleErrorRet(clSetKernelArg(kernel, 5, sizeof(arg5), (void *)&arg5));\
HandleErrorRet(clSetKernelArg(kernel, 6, sizeof(arg6), (void *)&arg6));\
HandleErrorRet(clEnqueueNDRangeKernel(command_queue, kernel, ndims, NULL, dimsize, dimwgsize, 0, NULL, event));

#define cl_call_profiling_8(kernel, command_queue, dimsize, dimwgsize, ndims, event, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7)\
HandleErrorRet(clSetKernelArg(kernel, 0, sizeof(arg0), (void *)&arg0));\
HandleErrorRet(clSetKernelArg(kernel, 1, sizeof(arg1), (void *)&arg1));\
HandleErrorRet(clSetKernelArg(kernel, 2, sizeof(arg2), (void *)&arg2));\
HandleErrorRet(clSetKernelArg(kernel, 3, sizeof(arg3), (void *)&arg3));\
HandleErrorRet(clSetKernelArg(kernel, 4, sizeof(arg4), (void *)&arg4));\
HandleErrorRet(clSetKernelArg(kernel, 5, sizeof(arg5), (void *)&arg5));\
HandleErrorRet(clSetKernelArg(kernel, 6, sizeof(arg6), (void *)&arg6));\
HandleErrorRet(clSetKernelArg(kernel, 7, sizeof(arg7), (void *)&arg7));\
HandleErrorRet(clEnqueueNDRangeKernel(command_queue, kernel, ndims, NULL, dimsize, dimwgsize, 0, NULL, event));
