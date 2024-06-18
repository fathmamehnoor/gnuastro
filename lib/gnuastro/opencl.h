#ifndef __GAL_OPENCL_H__
#define __GAL_OPENCL_H__

/* Include other headers if necessary here. Note that other header files
   must be included before the C++ preparations below. */
#include <gnuastro/data.h>
#include <CL/cl.h>
#define MAX_SOURCE_SIZE (0x100000)


/* When we are within Gnuastro's building process, 'IN_GNUASTRO_BUILD' is
   defined. In the build process, installation information (in particular
   'GAL_CONFIG_ARITH_CHAR' and the rest of the types that we needed in the
   arithmetic function) is kept in 'config.h'. When building a user's
   programs, this information is kept in 'gnuastro/config.h'. Note that all
   '.c' files must start with the inclusion of 'config.h' and that
   'gnuastro/config.h' is only created at installation time (not present
   during the building of Gnuastro). */
#ifndef IN_GNUASTRO_BUILD
#include <gnuastro/config.h>
#endif


/* C++ Preparations */
#undef __BEGIN_C_DECLS
#undef __END_C_DECLS
#ifdef __cplusplus
# define __BEGIN_C_DECLS extern "C" {
# define __END_C_DECLS }
#else
# define __BEGIN_C_DECLS                /* empty */
# define __END_C_DECLS                  /* empty */
#endif
/* End of C++ preparations */



/* Actual header contants (the above were for the Pre-processor). */
__BEGIN_C_DECLS  /* From C++ preparations */


char * getKernelSource(char *filename);

cl_kernel 
gal_gpu_kernel_create(char *name, char *function_name, 
                        cl_device_id device_id, cl_context *context, 
                        cl_command_queue *command_queue);



/*********************************************************************/
/*************            data transfer            *******************/
/*********************************************************************/
cl_mem
gal_gpu_copy_array_to_device(gal_data_t * in, cl_mem *input_mem_obj, cl_context context, 
                        cl_command_queue command_queue);

cl_mem
gal_gpu_copy_dsize_to_device(gal_data_t * in, cl_mem *input_mem_obj, cl_context context, 
                        cl_command_queue command_queue);

cl_mem
gal_gpu_copy_struct_to_device(gal_data_t *in, cl_mem *input_mem_obj, cl_context context, 
                        cl_command_queue command_queue);

void
gal_gpu_copy_from_device(gal_data_t *out, cl_mem *output_mem_obj, cl_command_queue command_queue);

__END_C_DECLS 

#endif


