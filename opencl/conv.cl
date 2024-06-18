

#include "conv_core.h"


__kernel void convolution(
    __global float * image,
    __global float * kernell,
	__global float * image_array,
    __global size_t * image_dsize,
    __global float * kernell_array,
    __global size_t * kernell_dsize,

	__global float * output
)
{	
    /* correct the pointers 
    image->dsize = image_dsize;
    image->array = image_array;
    kernell->dsize = kernell_dsize;
    kernell->array = kernell_array;*/

    /* get the image and kernel size */
    int image_height = image_dsize[0];
    int image_width = image_dsize[1];
    int kernell_height = kernell_dsize[0];
    int kernell_width = kernell_dsize[1];
   
    /* get the local group id */
    int id = get_global_id(0);
    int row = id/image_width;
    int col = id%image_width;



    // @@ conv_core.c @@ 

    CONV_CORE
}
