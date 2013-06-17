/*
 * file: cl_util.c
 * 
 * Implements cl_util.h
 *
 * Written by Mads Buvik Sandvei (https://github.com/madsbuvi/, madsbuvi@stud.ntnu.no)
 */

#include "cl_util.h"
#include <stdlib.h>
#include <stdio.h>

int8_t *readFile(const char *filename, size_t *len)
{

	FILE *file;
	uint8_t *buffer;
	uint64_t fileLen;

	//Open file
	file = fopen(filename, "rb");
	if (!file)
	{
		fprintf(stderr, "Unable to open file %s", filename);
		return NULL;
	}

	//Get file length
	fseek(file, 0, SEEK_END);
	fileLen=ftell(file);
	*len = fileLen;
	fseek(file, 0, SEEK_SET);

	//Allocate memory
	buffer=(uint8_t *)calloc(1,fileLen+1);
	if (!buffer)
	{
		fprintf(stderr, "Memory error!");
        fclose(file);
		return NULL;
	}
	//Read file contents into buffer
	fread(buffer, fileLen, 1, file);
	buffer[fileLen]='\0';
	fclose(file);
	return buffer;
}

const char* oclErrorString(cl_int error)
{
    static const char* errorString[] = {
        "CL_SUCCESS",
        "CL_DEVICE_NOT_FOUND",
        "CL_DEVICE_NOT_AVAILABLE",
        "CL_COMPILER_NOT_AVAILABLE",
        "CL_MEM_OBJECT_ALLOCATION_FAILURE",
        "CL_OUT_OF_RESOURCES",
        "CL_OUT_OF_HOST_MEMORY",
        "CL_PROFILING_INFO_NOT_AVAILABLE",
        "CL_MEM_COPY_OVERLAP",
        "CL_IMAGE_FORMAT_MISMATCH",
        "CL_IMAGE_FORMAT_NOT_SUPPORTED",
        "CL_BUILD_PROGRAM_FAILURE",
        "CL_MAP_FAILURE",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "CL_INVALID_VALUE",
        "CL_INVALID_DEVICE_TYPE",
        "CL_INVALID_PLATFORM",
        "CL_INVALID_DEVICE",
        "CL_INVALID_CONTEXT",
        "CL_INVALID_QUEUE_PROPERTIES",
        "CL_INVALID_COMMAND_QUEUE",
        "CL_INVALID_HOST_PTR",
        "CL_INVALID_MEM_OBJECT",
        "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR",
        "CL_INVALID_IMAGE_SIZE",
        "CL_INVALID_SAMPLER",
        "CL_INVALID_BINARY",
        "CL_INVALID_BUILD_OPTIONS",
        "CL_INVALID_PROGRAM",
        "CL_INVALID_PROGRAM_EXECUTABLE",
        "CL_INVALID_KERNEL_NAME",
        "CL_INVALID_KERNEL_DEFINITION",
        "CL_INVALID_KERNEL",
        "CL_INVALID_ARG_INDEX",
        "CL_INVALID_ARG_VALUE",
        "CL_INVALID_ARG_SIZE",
        "CL_INVALID_KERNEL_ARGS",
        "CL_INVALID_WORK_DIMENSION",
        "CL_INVALID_WORK_GROUP_SIZE",
        "CL_INVALID_WORK_ITEM_SIZE",
        "CL_INVALID_GLOBAL_OFFSET",
        "CL_INVALID_EVENT_WAIT_LIST",
        "CL_INVALID_EVENT",
        "CL_INVALID_OPERATION",
        "CL_INVALID_GL_OBJECT",
        "CL_INVALID_BUFFER_SIZE",
        "CL_INVALID_MIP_LEVEL",
        "CL_INVALID_GLOBAL_WORK_SIZE",
    };

    const int errorCount = sizeof(errorString) / sizeof(errorString[0]);

    const int index = -error;

    return (index >= 0 && index < errorCount) ? errorString[index] : "";

}

void handle_error(cl_int error, int32_t line, const  char *file){
    fprintf(stderr,"Error encountered, code %d:\nAt line: %d\nIn file: %s\nError text:%s\n",
            error, line, file, oclErrorString(error));
}

int little_endian(){
	uint32_t endiantest = 1;
	uint8_t *endiantestptr = (uint8_t*)&endiantest;
	return endiantestptr[0];
}
void random_string(uint8_t *string, uint64_t len){
    static const char alphanum[] =
        "$%&'()*+,-./0123456789:;<=>?@"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"
        "abcdefghijklmnopqrstuvwxyz{|}";

	int i = 0;
    for (; i < len; ++i) {
        string[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }
}

uint32_t cl_get_num_gpus(){
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	uint32_t gpu_devices = 0;
	
	/* Get platforms */
	HandleErrorRet(clGetPlatformIDs(1, NULL, &ret_num_platforms));
	cl_platform_id platform_ids[ret_num_platforms];
	HandleErrorRet(clGetPlatformIDs(ret_num_platforms, platform_ids, &ret_num_platforms));
	
	/* Count number of GPU devices in each platform */
	int i = 0;
	for(; i < ret_num_platforms; i++){
		HandleErrorRet(clGetDeviceIDs(platform_ids[i], CL_DEVICE_TYPE_GPU, 0, NULL, &ret_num_devices));
		gpu_devices += ret_num_devices;
	}
	return gpu_devices;
}

int init_cl_gpu_specific(int32_t gpu_id, cl_device_id *device_id, cl_context *context, cl_command_queue *command_queue){
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	
	/* Get platforms */
	HandleErrorRet(clGetPlatformIDs(1, NULL, &ret_num_platforms));
	cl_platform_id platform_ids[ret_num_platforms];
	HandleErrorRet(clGetPlatformIDs(ret_num_platforms, platform_ids, &ret_num_platforms));
	
	/* Find the platform for the given device */
	int i = 0;
	for(; i < ret_num_platforms; i++){
		HandleErrorRet(clGetDeviceIDs(platform_ids[i], CL_DEVICE_TYPE_GPU, 0, NULL, &ret_num_devices));
		gpu_id -= ret_num_devices;
		if(gpu_id < 0){
			gpu_id += ret_num_devices;break;
		}
	}
	//Device didn't exist
	if(gpu_id>=ret_num_devices)return -1;
	
	cl_device_id device_ids[ret_num_devices];
	HandleErrorRet(clGetDeviceIDs(platform_ids[i], CL_DEVICE_TYPE_GPU, ret_num_devices, device_ids, &ret_num_devices));
	*device_id = device_ids[gpu_id];
	
	struct context_property {
		cl_context_properties property;
		cl_platform_id id;
		uint64_t terminate;
	};
	
	struct context_property cp = {CL_CONTEXT_PLATFORM, platform_ids[i], 0};
	
	/* Create OpenCL context */
	HandleErrorPar(*context = clCreateContext(&cp, 1, device_id, NULL, NULL, HANDLE_ERROR));

	/* Create Command Queue */
	HandleErrorPar(*command_queue = clCreateCommandQueue(*context, *device_id, 0, HANDLE_ERROR));
	
	return 0;
}

void init_cl_gpu_single(cl_device_id *device_id, cl_context *context, cl_command_queue *command_queue){
    /*
     * Set-up OpenCL
     */
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	cl_platform_id platform_id = NULL;
	/* Get Platform and Device Info */

	HandleErrorRet(clGetPlatformIDs(1, &platform_id, &ret_num_platforms));
	HandleErrorRet(clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, device_id, &ret_num_devices));

	/* Create OpenCL context */
	HandleErrorPar(*context = clCreateContext(NULL, 1, device_id, NULL, NULL, HANDLE_ERROR));
	/* Create Command Queue */
	HandleErrorPar(*command_queue = clCreateCommandQueue(*context, *device_id, 0, HANDLE_ERROR));
}

void init_cl_gpu_single_profiling(cl_device_id *device_id, cl_context *context, cl_command_queue *command_queue){
    /*
     * Set-up OpenCL
     */
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	cl_platform_id platform_id = NULL;
	/* Get Platform and Device Info */

	HandleErrorRet(clGetPlatformIDs(1, &platform_id, &ret_num_platforms));
	HandleErrorRet(clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, device_id, &ret_num_devices));

	/* Create OpenCL context */
	HandleErrorPar(*context = clCreateContext(NULL, 1, device_id, NULL, NULL, HANDLE_ERROR));

	/* Create Command Queue */
	HandleErrorPar(*command_queue = clCreateCommandQueue(*context, *device_id, CL_QUEUE_PROFILING_ENABLE, HANDLE_ERROR));
}


void cl_build(char **sources, int n_sources, cl_context context, cl_device_id device_id, cl_program *program)
{	
	const char *sources_read[n_sources];
	size_t lengths[n_sources];
	int i = 0;
	for(; i < n_sources; i++) sources_read[i] = (char *)readFile(sources[i], &lengths[i]);

	HandleErrorPar(*program = clCreateProgramWithSource(context, n_sources, sources_read,
		lengths, HANDLE_ERROR));
	
	/* Build Kernel Program */
	cl_int ret = clBuildProgram(*program, 1, &device_id, "", NULL, NULL);
	if(ret){
	    char buildString[100000];
	    buildString[0]='\0';
        HandleErrorRet(clGetProgramBuildInfo(*program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(char) * 100000, buildString, NULL));
        printf("build log:\n%s\n", buildString);
		pthread_exit((void *)oclErrorString(ret));
	}
	
}

cl_mem __cl_malloc__(cl_context context, size_t size, int line, const  char *file){
	cl_mem memory_object;

	cl_int error;
	memory_object = clCreateBuffer(context, CL_MEM_READ_WRITE, size, NULL, &error);
	if(error){
		handle_error(error, line, file);
		return 0;
	}
	return memory_object;
}

void __cl_copy_to__(cl_mem to, void *from, size_t size, size_t offset, cl_command_queue command_queue, int line, const char *file)
{
	
    cl_int error = clEnqueueWriteBuffer(command_queue, to, CL_TRUE, offset, size, from, 0, NULL, NULL);
    if(error){
        handle_error(error, line, file);
		pthread_exit((void *)oclErrorString(error));
    }
}

void __cl_copy_from__(cl_mem from, void *to, size_t size, size_t offset, cl_command_queue command_queue, int line, const char *file)
{
	
    cl_int error = clEnqueueReadBuffer(command_queue, from, CL_TRUE, offset, size, to, 0, NULL, NULL);
    if(error){
        handle_error(error, line, file);
        pthread_exit((void *)oclErrorString(error));
    }
}

cl_kernel __cl_create_kernel__(cl_program program, const char *kernelname, int line, const char *file){
    cl_int error;
	cl_kernel result = clCreateKernel(program, kernelname, &error);
    if(error){
        handle_error(error, line, file);
        pthread_exit((void *)oclErrorString(error));
    }
	return result;
}

size_t __cl_get_info__(cl_device_id device_id, cl_device_info info, int line, const char *file){
	size_t ret;
    cl_int error = clGetDeviceInfo(device_id, 
								info, sizeof(ret), 
								&ret, NULL);
    if(error){
        handle_error(error, line, file);
        pthread_exit((void *)oclErrorString(error));
    }
	return ret;
}