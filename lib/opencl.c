#include <config.h>

#include <stdio.h>
#include <errno.h>
#include <error.h>
#include <float.h>
#include <stdlib.h>

#include <CL/cl.h>
#include <gnuastro/data.h>

#include <gnuastro/opencl.h>
#include <string.h>
 char * getKernelSource(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
         if (!file) {
        fprintf(stderr, "Error: Could not open kernel source file '%s'\n", filename);
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    }
    fseek(file, 0, SEEK_END);
    int len = ftell(file) + 1;
    rewind(file);

    char *source = (char *)calloc(sizeof(char), len);
    if (!source)
    {
        fprintf(stderr, "Error: Could not allocate memory for source string\n");
        exit(EXIT_FAILURE);
    }
    fread(source, sizeof(char), len, file);
    fclose(file);
    return source;
}


/*********************************************************************/
/*************             Initialization          *******************/
/*********************************************************************/
/* Initializes the context and command queue with one of the available
   devices, selected using device_id. Then creates the kernel object
   using the created context for a specific funtion (named function_name)  */
cl_kernel 
gal_gpu_kernel_create(char *kernel_name, char *function_name,
                        cl_device_id device_id, cl_context *context, 
                        cl_command_queue *command_queue)
{
    /* initializations */
    int ret=0;
    cl_program program;
    char device_name[1024];
    char *kernelsource;
    cl_platform_id platform_id = NULL; 

    /* query available devices and select the one with device_id*/
    // just put one availbale platform inside platform_id 
    
    /* Set up platform and GPU device */
    cl_uint numPlatforms;

    /* Find number of platforms */
    ret = clGetPlatformIDs(0, NULL, &numPlatforms);
    if (numPlatforms == 0)
    {
        printf("Found 0 platforms!\n");
       
    }

    /* Get all platforms */
    cl_platform_id Platform[numPlatforms];
    ret = clGetPlatformIDs(numPlatforms, Platform, NULL);
    if(ret != CL_SUCCESS){
        printf("NO PLATFORM/n");
    }

    // Secure a device
    for (int i = 0; i < numPlatforms; i++)
    {
        ret = clGetDeviceIDs(Platform[i],  CL_DEVICE_TYPE_DEFAULT, 1, &device_id, NULL);
        if (ret == CL_SUCCESS)
        {
            break;
        }
    }

    clGetDeviceInfo(device_id, CL_DEVICE_NAME, sizeof(device_name), &device_name, NULL);
    printf("Using device: %s\n", device_name);

    *context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);
     
   
      *command_queue = clCreateCommandQueueWithProperties(*context, device_id, 0, &ret);
  
  //  *command_queue = clCreateCommandQueueWithProperties(*context, device_id, 0, &ret);
 


    // FILE *coreFile = fopen(core_name, "r");
    // FILE *kernelFile = fopen(kernel_name, "r");

    // char *result = (char *)malloc(MAX_SOURCE_SIZE); 
    // size_t core_size = fread(result, 1, MAX_SOURCE_SIZE, coreFile);
    
    // result[core_size] = '\n';
    // size_t kernel_size = fread(result + core_size+1, 1, MAX_SOURCE_SIZE, kernelFile);
    // result[core_size + kernel_size] = '\0';

    // fclose(coreFile);
    // fclose(kernelFile);


   kernelsource = getKernelSource(kernel_name);


    program = clCreateProgramWithSource(*context, 1, 
                                        (const char **)kernelsource, NULL, &ret);

    ret = clBuildProgram(program, 1, &device_id, "-I .", NULL, NULL);
     if(ret!=CL_SUCCESS){
        size_t log_size;
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

    char *log = (char *)malloc(log_size);
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);

    // Print the compilation log
    printf("Compilation log:\n%s\n", log);

    // Free allocated memory
    free(log);
    }

    cl_kernel kernel = clCreateKernel(program, function_name, &ret);

    return kernel;
    
    }




// void
// gal_gpu_context_prepare(cl_device_id device_id, cl_context *context, cl_command_queue *command_queue)
// {

//     cl_platform_id platform_id = NULL;   
//     cl_uint ret_num_devices;
//     cl_uint ret_num_platforms;

//     cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
//     ret = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);
 
//     *context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);
 
//     *command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

// }





cl_mem
gal_gpu_copy_array_to_device(gal_data_t *in, cl_mem *input_mem_obj, cl_context context, 
                            cl_command_queue command_queue)
{
    int ret=0;
    *input_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, 
                                    in->size * gal_type_sizeof(in->type), NULL, &ret);
                                    
    ret = clEnqueueWriteBuffer(command_queue, *input_mem_obj, CL_TRUE, 0,
                                in->size * gal_type_sizeof(in->type), (float *)in->array, 0, NULL, NULL);
    if(ret != CL_SUCCESS){
        printf("enwueing buffer error/n");
    }


}





cl_mem
gal_gpu_copy_dsize_to_device(gal_data_t *in, cl_mem *input_mem_obj, cl_context context, 
                            cl_command_queue command_queue)
{
    int ret=0;
    *input_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, 
                                    3*sizeof(size_t), NULL, &ret);

    ret = clEnqueueWriteBuffer(command_queue, *input_mem_obj, CL_TRUE, 0, 
                                3*sizeof(size_t), in->dsize, 0, NULL, NULL);

     if(ret != CL_SUCCESS){
        printf("enwueing buffer error/n");
    }
}





cl_mem
gal_gpu_copy_struct_to_device(gal_data_t *in, cl_mem *input_mem_obj, cl_context context, 
                            cl_command_queue command_queue)
{
    int ret=0;
    *input_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, 
                                    sizeof(in), NULL, &ret);

    ret = clEnqueueWriteBuffer(command_queue, *input_mem_obj, CL_TRUE, 0, 
                                sizeof(in), in, 0, NULL, NULL);


     if(ret != CL_SUCCESS){
        printf("enwueing buffer error/n");
    }
}





void
gal_gpu_copy_from_device(gal_data_t *out, cl_mem *output_mem_obj, cl_command_queue command_queue)
{
    clEnqueueReadBuffer(command_queue, *output_mem_obj, CL_TRUE, 0, 
                        out->size*gal_type_sizeof(out->type) , (float*)out->array, 0, NULL, NULL);
}
