/*********************************************************************
Permutation -- Work on permutations (arrays of indexs).
This is part of GNU Astronomy Utilities (Gnuastro) package.

Original author:
     Mohammad Akhlaghi <mohammad@akhlaghi.org>
Contributing author(s):
Copyright (C) 2017-2023 Free Software Foundation, Inc.

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
#include <string.h>
#include <stdlib.h>

#include <gnuastro/pointer.h>
#include <gnuastro/permutation.h>



/*********************************************************************/
/***************          Permutation info         *******************/
/*********************************************************************/
void
gal_permutation_check(size_t *permutation, size_t size)
{
  size_t i;
  for(i=0; i<size; ++i)
    printf("after[ %-5zu ]    =   before [ %-5zu ]\n", i, permutation[i]);
}




















/*********************************************************************/
/***************          Apply permutation        *******************/
/*********************************************************************/
/* Re-order the input dataset based on the given permutation. If
   'permutation' is NULL, then the input won't be touched (no re-ordering).

   This is a re-implementation of GSL's 'gsl_permute' function (from its
   'permutation/permute_source.c'). The reason we didn't use that function
   was that it uses system-specific types (like 'long' and 'int') which are
   not easily convertable to Gnuastro's width-based types. There is also a
   separate function for each type, heavily using macros to allow a "base"
   function to work on all the types. Thus it is hard to
   read/understand. Since we use fixed-width types, we can easily use
   'memcpy' and have a type-agnostic implementation (only needing the width
   of the type).

   As described in GSL's source code and manual, this implementation comes
   from Knuth's "Art of computer programmin" book, the "Sorting and
   Searching" chapter of Volume 3 (3rd ed). Section 5.2 Exercise 10
   (answers), p 617. See there fore more explanations. The algorithm is a
   little too abstract, but memory and CPU efficient.

   Definition of permutations:

      permute:    OUT[ i       ]   =   IN[ perm[i] ]     i = 0 .. N-1
      inverse:    OUT[ perm[i] ]   =   IN[ i       ]     i = 0 .. N-1
*/
static void
permutation_apply_raw(gal_data_t *input, size_t *permutation,
                      int onlydim0)
{
  void *tmp;
  uint8_t *array=input->array;
  size_t i, k, pk, winc, width, size, increment;

  /* If 'onlydim0' is given and the input has more than one dimension, we
     need to permute less (only along the 0th dimension). */
  if(onlydim0 && input->ndim>1)
    { size=input->dsize[0]; increment=input->size/size; }
  else { size=input->size; increment=1; }

  /* If permutation is NULL, then it is assumed that the data doesn't need
     to be re-ordered. */
  if(permutation)
    {
      /* Necessary initializations. */
      width=gal_type_sizeof(input->type);
      tmp=gal_pointer_allocate(input->type, increment, 0, __func__, "tmp");

      /* Do the permutation. */
      winc=width*increment;
      for(i=0;i<size;++i)
        {
          k=permutation[i];

          while(k>i) k=permutation[k];

          if(k>=i)
            {
              pk = permutation[k];
              if( pk != i )
                {
                  memcpy(tmp, &array[i*winc], winc);

                  while(pk!=i)
                    {
                      memcpy(&array[k*winc], &array[pk*winc], winc);
                      k  = pk;
                      pk = permutation[k];
                    }

                  memcpy(&array[k*winc], tmp, winc);
                }
            }
        }

      /* Clean up. */
      free(tmp);
    }
}





/* Apply the inverse of given permutation on the input dataset, see
   'gal_permutation_apply_inverse'. */
void
gal_permutation_apply_inverse(gal_data_t *input, size_t *permutation)
{
  void *tmp, *ttmp;
  size_t i, k, pk, width;
  uint8_t *array=input->array;

  if(permutation)
    {
      /* Initializations. */
      width=gal_type_sizeof(input->type);
      tmp=gal_pointer_allocate(input->type, 1, 0, __func__, "tmp");
      ttmp=gal_pointer_allocate(input->type, 1, 0, __func__, "ttmp");

      /* Re-order the values. */
      for(i=0;i<input->size;++i)
        {
          k=permutation[i];

          while(k>i) k=permutation[k];

          if(k>=i)
            {
              pk = permutation[k];

              if(pk!=i)
                {
                  memcpy(tmp, &array[k*width], width);

                  while(pk!=i)
                    {
                      memcpy(ttmp, &array[pk*width], width);
                      memcpy(&array[pk*width], tmp, width);
                      memcpy(tmp, ttmp, width);
                      k  = pk;
                      pk = permutation[k];
                    }

                  memcpy(&array[pk*width], tmp, width);
                }
            }
        }

      /* Clean up. */
      free(tmp);
      free(ttmp);
    }
}





void
gal_permutation_apply(gal_data_t *input, size_t *permutation)
{ permutation_apply_raw(input, permutation, 0); }

void
gal_permutation_apply_onlydim0(gal_data_t *input, size_t *permutation)
{ permutation_apply_raw(input, permutation, 1); }





/* Transpose square (2d) input. */
static void
permutation_transpose_2d_square(gal_data_t *input)
{
  void *a, *b, *swap;
  size_t width=input->dsize[0];
  size_t i, j, nbytes=gal_type_sizeof(input->type);

  /* Allocate the SWAP space, we are just allocating an 64-bit integer for
     its storage. */
  swap=gal_pointer_allocate(GAL_TYPE_UINT64, 1, 0, __func__, "swap");

  /* Go over the elements. */
  for(i=0;i<width;++i)
    for(j=i+1;j<width;++j)
      {
        /* For easy reading. */
        a=gal_pointer_increment(input->array, i*width+j, input->type);
        b=gal_pointer_increment(input->array, j*width+i, input->type);

        /* Copy the "to" value into the swap, then copy the "from" value
           into "to" and finally copy swap into "from". */
        memcpy(swap, a,    nbytes);
        memcpy(a,    b,    nbytes);
        memcpy(b,    swap, nbytes);
      }

  /* Clean up. */
  free(swap);
}





/* Transpose square (2d) input. */
static void
permutation_transpose_2d_rectangle(gal_data_t *input)
{
  void *a, *b;
  size_t i, j, nbytes;
  gal_data_t *out=NULL;
  size_t *id=input->dsize, od[2]={id[1], id[0]};

  /* Moving values in memory is only necessary when the 0-th dimension has
     more than one element. */
  if(input->dsize[0]>1)
    {
      /* Allocate the output array. */
      out=gal_data_alloc(NULL, input->type, 2, od, NULL, 0,
                         input->minmapsize, input->quietmmap,
                         NULL, NULL, NULL);

      /* Go over the elements and put them in the proper place of the
         output. */
      nbytes=gal_type_sizeof(input->type);
      for(i=0;i<id[0];++i)
        for(j=0;j<id[1];++j)
          {
            /* For easy reading. */
            a=gal_pointer_increment(input->array, i*id[1]+j, input->type);
            b=gal_pointer_increment(out->array,   j*od[1]+i, input->type);

            /* Copy the input ('a' pointer) to output ('b') pointer. */
            memcpy(b, a, nbytes);
          }

      /* Free the original input pointer and replace it with the output
         array, then free the output. */
      free(input->array);
      input->array=out->array;
      out->array=NULL;
      gal_data_free(out);
    }

  /* Update the dimesions. */
  input->dsize[0]=od[0];
  input->dsize[1]=od[1];
}





/* Transpose a 2D dataset. */
void
gal_permutation_transpose_2d(gal_data_t *input)
{
  uint8_t type;
  size_t nbytes;

  /* Sanity checks, see the comment below. */
  type=input->type;
  nbytes=gal_type_sizeof(type);
  if(nbytes>8)
    error(EXIT_FAILURE, 0, "%s: a bug! Please contact us at '%s' to "
          "find the cause and fix this problem. This function currently "
          "assumes the largest possible type size is 8 bytes, but the "
          "requested '%s' type needs %zu bytes", __func__,
          PACKAGE_BUGREPORT, gal_type_name(type, 1), nbytes);
  if(input->ndim!=2)
    error(EXIT_FAILURE, 0, "%s: only 2D inputs are supported", __func__);

  /* A square array can be transposed much more easier and faster than a
     non-square array. */
  if(input->dsize[0]==input->dsize[1])
    permutation_transpose_2d_square(input);
  else
    permutation_transpose_2d_rectangle(input);
}
