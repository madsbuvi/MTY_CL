/*! \file cl_util.h
 *	\brief Header for the some OpenCL utility functions and macros
 *
 *	A set of convenience macros and functions for OpenCL, written to make the API
 *	less cumbersome for me to use.
 *	no functions defined here return error codes, instead errors are printed and
 *	the thread exits.
 *
 *	\authors Mads Buvik Sandvei
 *	\date 2013
 *
 */ 
#include <stdint.h>
#include <CL/cl.h>
#include <pthread.h>

/*! \brief Performs a binary read of the given file
 *
 * 	Performs a binary read of the given file.
 *	Natural part of cl util because when using opencl you frequently need to read in whole source files
 * 	and keep them in a buffer.
 *
 *	\param[in] filename path to the file to be read.
 *	\param[out]	len	pointer to a size_t variable to store the length of the file read in.
 *	\return	A pointer to a newly allocated buffer with the full contents of the file if successful, NULL otherwise.
 */
int8_t *readFile(const char *filename, size_t *len);

/*! \brief Get an error string based on error code
 *
 *  Function to get an error string based on error code
 *  Copied from enj @ enja.org
 *	
 *	\param[in] error an OpenCL error code
 *	\return A const string that gives a textual representation of the error.
 */
const char* oclErrorString(cl_int error);

/*! \brief Function to "handle" an error code.
 *
 *  Does nothing other than print out the oclErrorString along with location in source of the error.
 *
 *	\param[in] error an OpenCL error code
 *	\param[in] line Line number where the error occured
 *	\param[in] file Filename of the file where the error occured
 */
void handle_error(cl_int error, int32_t line, const  char *file);

//Defines to handle errors:
//You do not need to use semicolons after these defines, but it's a good habit.

/*!	\brief Define to wrap around a function that returns error code
 *  Define to wrap around a function that returns error code
 *  E.g. HandleErrorRet(clGetPlatformIDs(1, &platform_id, &ret_num_platforms));
 *	Call handle_error on a returned error code and then exits the current thread.
 */
#define HandleErrorRet(clfunc)\
{\
    cl_int error = (clfunc);\
    if(error){\
        handle_error(error, __LINE__, __FILE__);\
		unregister_gpu();\
        pthread_exit((void *)oclErrorString(error));\
    }\
}

//Defines to handle errors:
//You do not need to use semicolons after these defines, but it's a good habit.

/*!	\brief Define to wrap around a function that returns error code but does NOT quit
 *  Define to wrap around a function that returns error code
 *  E.g. HandleErrorRet(clGetPlatformIDs(1, &platform_id, &ret_num_platforms));
 *	Call handle_error on a returned error code and then continues executing.
 */
#define HandleErrorRet_ignore(clfunc)\
{\
    cl_int error = (clfunc);\
    if(error){\
        handle_error(error, __LINE__, __FILE__);\
		unregister_gpu();\
    }\
}

//! \brief Alias of the error parameter for HandleErrorPar
#define HANDLE_ERROR &error
/*!	\brief Define to wrap around a function that returns error code to a parameter
 *  Define to wrap around a function that returns error code to a parameter
 *  MAKE SURE YOU PASS "HANDLE_ERROR" AS THE ERROR RETURN PARAMETER
 *	Macro should encompass the entire statement.
 *  E.G. HandleErrorPar(context = clCreateContext(NULL, 1, &device_id, NULL, NULL, HANDLE_ERROR));
 */
#define HandleErrorPar(clfunc)\
{\
    cl_int error;\
    (clfunc);\
    if(error){\
        handle_error(error, __LINE__, __FILE__);\
		unregister_gpu();\
        pthread_exit((void *)oclErrorString(error));\
    }\
}


/*! \brief Determines whether the host is big or little endian
 *	\return 1 if little endian, 0 if big endian
 */
int little_endian();


/*! \Brief Random number
 * Define to wrap around the convoluted method of getting a truly random value from 0 to interval from the rand() function
 */
#define random(interval) ((int)((double)rand() / ((double)RAND_MAX + 1) * interval))

/*! \brief Random string
 * Function to generate a random ASCII string of length len. Does not allocate its own memory.
 */
void random_string(uint8_t *string, uint64_t len);

/*! \brief Initializes a single GPU
 *
 *  Initializes a single context for GPU. Uses whatever device it finds first.
 *
 *	\param[out] device_id The device ID
 *	\param[out] context The context
 *	\param[out] command_queue The command queue
 */
void init_cl_gpu_single(cl_device_id *device_id, cl_context *context, cl_command_queue *command_queue);

/*! \brief Initializes a single GPU with profiling enabled
 *
 *  Initializes a single context for GPU. Uses whatever device it finds first.
 *
 *	\param[out] device_id The device ID
 *	\param[out] context The context
 *	\param[out] command_queue The command queue
 */
void init_cl_gpu_single_profiling(cl_device_id *device_id, cl_context *context, cl_command_queue *command_queue);
 
/*! \brief Initializes a specific GPU
 *
 *	\param[in]	gpu_id the gpu to initialize, should be less than cl_get_num_gpus()
 *	\param[out] device_id The device ID
 *	\param[out] context The context
 *	\param[out] command_queue The command queue
 */
int init_cl_gpu_specific(int32_t gpu_id, cl_device_id *device_id, cl_context *context, cl_command_queue *command_queue);
 

 
 
/*! \brief Builds the variable length list of sources as a concatenated source.
 *	\param[in] context A context
 *	\param[in] device_id A device id
 *	\param[out] program The rogram
 *	\param[in] ... The list of sources
 */
#define cl_build_all(context, device_id, program_ptr, ...)\
 {\
	char *sources[] = {__VA_ARGS__};\
	int n_sources = sizeof(sources)/sizeof(char *);\
	cl_build(sources, n_sources, context, device_id, program_ptr);\
}

/*! \brief Builds a list of sources into a program
 *
 *	Should not be called directly, rather through the cl_build_all macro
 *	\param[in] sources list of sources
 *	\param[in] n_sources number of sources
 *	\param[in] context A context
 *	\param[in] device_id A device id
 *	\param[out] program The rogram
 */
void cl_build(char **sources, int n_sources, cl_context context, cl_device_id device_id, cl_program *program);

/*! \brief Creates an OpenCL kernel
 *
 *	\param[in] program The program to get the kernel from
 *	\param[in] kernelname The name of the kernel to create
 */
#define cl_create_kernel(program, kernelname) __cl_create_kernel__(program, kernelname, __LINE__, __FILE__)
/*! \brief Builds a list of sources into a program
 *
 *	Should not be called directly, rather from the cl_create_kernel() macro.
 *	\param[in] program The program to get the kernel from
 *	\param[in] kernelname The name of the kernel to create
 *	\param[in] line The line the code invoked from
 *	\param[in] file The file the code was invoked from
 */
cl_kernel __cl_create_kernel__(cl_program program, const char *kernelname, int line, const char *file);

/*! \brief good old malloc for opencl
 *
 *	\param[in] context A context
 *	\param[in] size	How much to allocate
 *	\return A cl_mem object if successful, 0 otherwise.
 */
#define cl_malloc(context, size) __cl_malloc__(context, size, __LINE__, __FILE__)

/*! \brief good old malloc for opencl
 *
 *	Should not be called directly, rather through the cl_malloc macro
 *
 *	\param[in] context A context
 *	\param[in] size	How much to allocate
 *	\param[in] line The line the code invoked from
 *	\param[in] file The file the code was invoked from
 *	\return A cl_mem object if successful, 0 otherwise.
 */
cl_mem __cl_malloc__(cl_context context, size_t size, int line, const  char *file);

/*! \brief Memcpy for opencl
 *
 *
 *	\param[in] to The memory object to copy to
 *	\param[in] from	The host data to copy from
 *	\param[in] size How much to copy
 *	\param[in] offset	Offset into the cl_mem object
 *	\param[in] queue	A command queue
 */
#define cl_copy_to(to, from, size, offset, queue) __cl_copy_to__(to, from, size, offset, queue, __LINE__, __FILE__)

/*! \brief Memcpy for opencl
 *
 *	Should not be called directly, rather from the cl_copy_to() macro.
 *
 *	\param[in] to The memory object to copy to
 *	\param[in] from	The host data to copy from
 *	\param[in] size How much to copy
 *	\param[in] offset	Offset into the cl_mem object
 *	\param[in] queue	A command queue
 *	\param[in] line The line the code invoked from
 *	\param[in] file The file the code was invoked from
 */
void __cl_copy_to__(cl_mem to, void *from, size_t size, size_t offset, cl_command_queue command_queue, int line, const char *file);


/*! \brief Memcpy for opencl
 *
 *
 *	\param[in] from The memory object to copy from
 *	\param[in] to	The host data to copy to
 *	\param[in] size How much to copy
 *	\param[in] offset	Offset into the cl_mem object
 *	\param[in] queue	A command queue
 */
#define cl_copy_from(from, to, size, offset, queue) __cl_copy_from__(from, to, size, offset, queue, __LINE__, __FILE__)

/*! \brief Memcpy for opencl
 *
 *	Should not be called directly, rather from the cl_copy_from() macro.
 *
 *	\param[in] from The memory object to copy from
 *	\param[in] to	The host data to copy to
 *	\param[in] size How much to copy
 *	\param[in] offset	Offset into the cl_mem object
 *	\param[in] queue	A command queue
 *	\param[in] line The line the code invoked from
 *	\param[in] file The file the code was invoked from
 */
void __cl_copy_from__(cl_mem from, void *to, size_t size, size_t offset, cl_command_queue command_queue, int line, const char *file);

/*! \brief Info functions
 *
 *
 *	\param[in] devid A device ID
 *	\param[in] ifno The info to retrieve
 *	\param[in] line The line the code invoked from
 *	\param[in] file The file the code was invoked from
 */
#define cl_get_info(devid, info) __cl_get_info__(devid, info, __LINE__, __FILE__)

/*! \brief Info functions
 *
 *	Should not be called directly, rather from the cl_get_info() macro.
 *
 *	\param[in] devid A device ID
 *	\param[in] ifno The info to retrieve
 *	\param[in] line The line the code invoked from
 *	\param[in] file The file the code was invoked from
 */
size_t __cl_get_info__(cl_device_id device_id, cl_device_info info, int line, const char *file);

/*! \Brief gets the number of gpus on the system
 *
 *	\return the number of gpus
 */
uint32_t cl_get_num_gpus();

/*! \brief Opencl function call macros
 *
 * Opencl function call macros
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
