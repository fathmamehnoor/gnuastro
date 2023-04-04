/*********************************************************************
Pool - Pool input data and create a new dataset.
Pool is part of GNU Astronomy Utilities (Gnuastro) package.

Original author:
     Faezeh Bidjarchian <fbidjarchian@gmail.com>
Contributing author(s):
     Mohammad Akhlaghi <mohammad@akhlaghi.org>
Copyright (C) 2023-2023 Free Software Foundation, Inc.

Gnuastro is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

Gnuastro is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with Gnuastro. If not, see <http://www.gnu.org/licenses/>.
**********************************************************************/
#include <config.h>

#include <stdio.h>
#include <errno.h>
#include <error.h>
#include <stdlib.h>
#include <string.h>

#include <gnuastro/wcs.h>
#include <gnuastro/type.h>
#include <gnuastro/fits.h>
#include <gnuastro/pool.h>
#include <gnuastro/pointer.h>
#include <gnuastro/threads.h>
#include <gnuastro/dimension.h>
#include <gnuastro/statistics.h>

#include <gnuastro-internal/checkset.h>





/* Identifiers for each operator. */
enum pool_operators
{
  POOL_MAX,      /* Maximum the desired pixels.  */
  POOL_MIN,      /* Minimum the desired pixels.  */
  POOL_SUM,      /* Sum of desired pixels.       */
  POOL_MEAN,     /* Mean the desired pixel.      */
  POOL_MEDIAN,   /* Median the desired pixels.   */
};





/**********************************************************************/
/****************             Pooling                 *****************/
/**********************************************************************/
#define POOLING_DIM 10

/* Main input/output parameters. */
struct pooling
{
  int       operator;     /* The type of pooling.             */
  size_t    poolsize;     /* The size of pooling.             */
  size_t      *osize;     /* The output size.                 */
  gal_data_t  *input;     /* Dataset to print values of.      */
  gal_data_t    *out;     /* Output data structure.           */
};





/* Do the pooling on each thread.

   Current assumptions:

   - The size of pooling can be every single number (the pooling window is
     a square).
   - The width and height of the input are not necessarily divisible
     by the size of the pooling. In other words, the image can be both
     square and rectangular.
   - We apply the pooling to our input with a stride of poolsize. So,
     for example the stride is 2 when poolsize is equal to 2. */
static void *
pool_type_on_thread(void *in_prm)
{
  struct gal_threads_params *tprm=(struct gal_threads_params *)in_prm;
  struct pooling *pp=(struct pooling *)tprm->params;
  gal_data_t *input=pp->input;

  size_t pools= pp->poolsize;
  size_t ndim=input->ndim;
  size_t w=input->dsize[1], wr;
  size_t h=input->dsize[0], hr;
  gal_data_t *statv=NULL, *result=NULL;
  size_t i, a, b, oind, iind, vc, numpixs, coord[POOLING_DIM], index;

  /* All number of pixels that we selected each time par the pooling. */
  numpixs=pools*pools;

  /* Allocated memory for selected pixels of input to feed into the
     statisitical operation. */
  statv=gal_data_alloc(NULL, input->type, 1, &numpixs, NULL, 0,
                       input->minmapsize, input->quietmmap,
                       NULL, NULL, NULL);

  /* Go over all the pixels that were assigned to this thread. */
  for(i=0; tprm->indexs[i] != GAL_BLANK_SIZE_T; ++i)
    {
      /* For easy reading, put the index in 'ind'. */
      oind=tprm->indexs[i];

      /* Get the coordinate of the pixel. */
      gal_dimension_index_to_coord(oind, ndim, pp->osize, coord);

      /* Convert the pixel coordinate to the desired pixel that we must
         select to set the pooling's starting pointer. */
      iind=pools*w*coord[0]+pools*coord[1];
      hr=iind%h;
      wr=iind%w;

      /* In some cases, the pooling window doesn't cover a whole squared
         window and only has maybe one pixel. So since we initialize the
         statv by Null (=0 in C), some of these values fill with input
         values and others remain zero. So these zeros will affect the
         outputs.  Therefore we initialize the statv by blank value. */
      gal_blank_initialize(statv);

      /* Set the sorted and blank check flags to 0 so the statistical
         operators re-check everytime for sorted or blank elements. */
      statv->flag=0;

      /* Depending on the size of pooling, we condider a set of pixels
	 and fill the temporary 'values' array. Then we do the required
         operation on them. */
      vc=0;
      for(a=0;a<pools && hr+a<=input->dsize[0];++a)
	for(b=0;b<pools && wr+b<input->dsize[1];++b)
	  {
	    index=iind + a*w + b;
	    if (index<input->size)
	      {
                memcpy(gal_pointer_increment(statv->array, vc,
                                             statv->type),
                       gal_pointer_increment(input->array, index,
                                             input->type),
                       gal_type_sizeof(input->type));
                vc++;
	      }
	  }

      /* Do the necessary calculation. */
      switch(pp->operator)
        {
        case POOL_MAX:    result=gal_statistics_maximum(statv);   break;
        case POOL_MIN:    result=gal_statistics_minimum(statv);   break;
        case POOL_SUM:    result=gal_statistics_sum(statv);       break;
        case POOL_MEAN:   result=gal_statistics_mean(statv);      break;
        case POOL_MEDIAN: result=gal_statistics_median(statv, 1); break;
        default:
          error(EXIT_FAILURE, 0, "%s: a bug! Please contact us at %s "
                "to fix the problem. 'operator' code %d is not "
                "recognized.", PACKAGE_BUGREPORT, __func__,
                pp->operator);
        }

      /* Make sure the output array type and the type of result are the
         same. */
      if(result->type!=pp->out->type)
        result=gal_data_copy_to_new_type_free(result, pp->out->type);

      /* Copy the result into the output array. */
      memcpy(gal_pointer_increment(pp->out->array, oind, pp->out->type),
             result->array, gal_type_sizeof(pp->out->type));

      /* Clean up. */
      gal_data_free(result);
    }

  /* Clean up. */
  gal_data_free(statv);

  /* Wait for all the other threads to finish, then return. */
  if(tprm->b) pthread_barrier_wait(tprm->b);
  return NULL;
}





static gal_data_t *
pool_generic(gal_data_t *input, size_t psize, int operator, size_t numthreads)
{
  struct pooling pp={0};

  int otype=GAL_TYPE_INVALID;
  size_t outndim=input->ndim, i, r, diff;

  /* Print a warning if the psize has a wrong value. It happens when the
     user writes a negative value for the poolsize. */
  if(psize>(size_t)(-1)/2 || psize==0)
    error(EXIT_FAILURE, 0, "the value of poolsize must be positive, and "
          "non zero)");

  /* Make sure the given poolsize is lower than the input's width or
     hight. */
  if(psize>input->dsize[0] || psize>input->dsize[1])
    error(EXIT_FAILURE, 0, "%s: the pool size along dimension must be "
         "greater than the input's width or hight in that dimension",
         __func__);

  /* Set the pointers in the structure of the parameter. */
  pp.input=input;
  pp.poolsize=psize;
  pp.operator=operator;

  if(pp.input->size==1) pp.out=pp.input;
  else
    {
      /* Resize output when calculating pooling on it and the remainder
         is not zero. So we must calculate the pooling one more time for
         the remaining pixels. */
      pp.osize=gal_pointer_allocate(GAL_TYPE_SIZE_T, input->ndim, 0,
                                    __func__, "osize");

      /* if the width is not divisible by the poolsize, we have to add one
         dimension to the output dimension for these remaining pixels. */
      for(i=0;i<input->ndim;++i)
        {
          r=(input->dsize[i])%psize;
          diff=((r==0)?0:1);
          pp.osize[i]=(input->dsize[i]/psize)+diff;
        }

      /* Set the type of the output dataset. */
      switch(pp.operator)
        {
        case POOL_MAX:
        case POOL_MIN:
        case POOL_MEDIAN:
          otype=pp.input->type;
          break;

        case POOL_SUM:
        case POOL_MEAN:
          otype=GAL_TYPE_FLOAT64;
          break;

        default:
          error(EXIT_FAILURE, 0, "%s: a bug! please contact us at %s to fix "
                "the problem. The 'operator' code %d is not recognized",
                PACKAGE_BUGREPORT, __func__, operator);
        }

      /* Allocating an struct for the output data. */
      pp.out=gal_data_alloc(NULL, otype, outndim, pp.osize, NULL, 0,
                            pp.input->minmapsize, pp.input->quietmmap, NULL,
                            NULL, NULL);

      /* Spin-off the threads and do the processing on each thread. */
      gal_threads_spin_off(pool_type_on_thread, &pp, pp.out->size, numthreads,
                           pp.input->minmapsize, pp.input->quietmmap);
    }

  /* Correct the WCS (if it has one). */
  if(input->wcs)
    {
      /* We currently assume that a 'cdelt' exists (due to a lack of
         time)! */
      if(pp.input->wcs->cdelt==NULL)
        error(EXIT_FAILURE, 0, "%s: a bug! Please contact us at '%s' to "
              "fix the problem. The input WCS has no 'cdelt' component",
              __func__, PACKAGE_BUGREPORT);

      /* Copy the input WCS to the output and correct the values. */
      pp.out->wcs=gal_wcs_copy(pp.input->wcs);
      pp.out->wcs->crpix[0]/=psize;
      pp.out->wcs->crpix[1]/=psize;
      pp.out->wcs->cdelt[0]*=psize;
      pp.out->wcs->cdelt[1]*=psize;
    }

  /* Clean up and return. */
  free(pp.osize);
  return pp.out;
}





gal_data_t *
gal_pool_max(gal_data_t *input, size_t psize, size_t numthreads)
{
  return pool_generic(input, psize, POOL_MAX, numthreads);
}





gal_data_t *
gal_pool_min(gal_data_t *input, size_t psize, size_t numthreads)
{
  return pool_generic(input, psize, POOL_MIN, numthreads);
}





gal_data_t *
gal_pool_sum(gal_data_t *input, size_t psize, size_t numthreads)
{
  return pool_generic(input, psize, POOL_SUM, numthreads);
}





gal_data_t *
gal_pool_mean(gal_data_t *input, size_t psize, size_t numthreads)
{
  return pool_generic(input, psize, POOL_MEAN, numthreads);
}





gal_data_t *
gal_pool_median(gal_data_t *input, size_t psize, size_t numthreads)
{
  return pool_generic(input, psize, POOL_MEDIAN, numthreads);
}
