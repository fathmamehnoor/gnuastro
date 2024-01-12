/*********************************************************************
Functions for linked lists.
This is part of GNU Astronomy Utilities (Gnuastro) package.

Original author:
     Mohammad Akhlaghi <mohammad@akhlaghi.org>
Contributing author(s):
Copyright (C) 2015-2024 Free Software Foundation, Inc.

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
#include <assert.h>
#include <inttypes.h>

#include <gnuastro/list.h>
#include <gnuastro/blank.h>
#include <gnuastro/pointer.h>

#include <gnuastro-internal/checkset.h>









/****************************************************************
 *****************           String          ********************
 ****************************************************************/
void
gal_list_str_add(gal_list_str_t **list, char *value,
                 int allocate)
{
  gal_list_str_t *newnode;

  /* If the value is a NULL pointer, don't add to the list. */
  if(value==NULL) return;

  errno=0;
  newnode=malloc(sizeof *newnode);
  if(newnode==NULL)
    error(EXIT_FAILURE, errno, "%s: allocating new node", __func__);

  if(allocate)
    gal_checkset_allocate_copy(value, &newnode->v);
  else
    newnode->v=value;

  newnode->next=*list;
  *list=newnode;
}





char *
gal_list_str_pop(gal_list_str_t **list)
{
  char *out=NULL;
  gal_list_str_t *tmp;
  if(*list)
    {
      tmp=*list;
      out=tmp->v;
      *list=tmp->next;
      free(tmp);
    }
  return out;
}





size_t
gal_list_str_number(gal_list_str_t *list)
{
  size_t num=0;
  gal_list_str_t *tmp;
  for(tmp=list;tmp!=NULL;tmp=tmp->next) ++num;
  return num;
}





gal_list_str_t *
gal_list_str_last(gal_list_str_t *list)
{
  if(list)
    {
      while(list->next!=NULL) list=list->next;
      return list;
    }
  else return NULL;
}





void
gal_list_str_print(gal_list_str_t *list)
{
  gal_list_str_t *tmp;
  for(tmp=list; tmp!=NULL; tmp=tmp->next)
    printf("%s\n", tmp->v);
}





void
gal_list_str_reverse(gal_list_str_t **list)
{
  char *thisstring;
  gal_list_str_t *correctorder=NULL;

  /* Only do the reversal if there is more than one element. */
  if( *list && (*list)->next )
    {
      while(*list!=NULL)
        {
          thisstring=gal_list_str_pop(list);
          gal_list_str_add(&correctorder, thisstring, 0);
        }
      *list=correctorder;
    }
}





void
gal_list_str_free(gal_list_str_t *list, int freevalue)
{
  gal_list_str_t *tmp;
  while(list!=NULL)
    {
      tmp=list->next;
      if(freevalue)
        free(list->v);
      free(list);
      list=tmp;
    }
}




/* Replacement characters for commented space (ASCII code 14 for "Shift
   out"). These are chosen as non-printable ASCII characters, that user's
   will not be typing. Inspired from 'gal_options_parse_list_of_strings'. */
#define LIST_COMMENTED_SPACE 14
gal_list_str_t *
gal_list_str_extract(char *string)
{
  gal_list_str_t *list=NULL, *tmp;
  char *c, *d, *cp, *token, *saveptr, delimiters[]=" \t";

  /* Make a copy of the input string, remove all commented delimiters
     (those with a preceding '\'), this was inspired from
     'gal_options_parse_list_of_strings'. */
  gal_checkset_allocate_copy(string, &cp);
  for(c=cp; *c!='\0'; c++)
    if(*c=='\\' && c[1]!='\0')
      {
        /* If the next character (after the '\') is a delimiter, we need to
           replace it with a non-delimiter (and not-typed!) character and
           shift the whole string back by one character to simplify future
           steps. */
        if(c[1]==' ')
          {
            *c=LIST_COMMENTED_SPACE;
            for(d=c+2; *d!='\0'; ++d) {*(d-1)=*d;} *(d-1)='\0';
          }
      }

  /* Tokenize the string. */
  token=strtok_r(cp, delimiters, &saveptr);
  gal_list_str_add(&list, token, 1);
  while(token!=NULL)
    {
      token=strtok_r(NULL, delimiters, &saveptr);
      if(token!=NULL)
        gal_list_str_add(&list, token, 1);
    }

  /* Go over each token and make final corrections: */
  for(tmp=list;tmp!=NULL;tmp=tmp->next)
    {
      /* Change the temporarily replaced value to a SPACE. */
      for(c=tmp->v; *c!='\0'; ++c)
        if(*c==LIST_COMMENTED_SPACE)
          *c=' ';

      /* If the last character is a new-line character, set it to the end
         of the string. */
      if( tmp->v[strlen(tmp->v)-1]=='\n' )
        tmp->v[strlen(tmp->v)-1]='\0';
    }

  /* Return the list. */
  gal_list_str_reverse(&list);
  return list;
}





char *
gal_list_str_cat(gal_list_str_t *list, char delimiter)
{
  size_t bsize=0;
  char *c, *o, *out;
  gal_list_str_t *tmp;

  /* If the list is empty, return a NULL pointer. */
  if(list==NULL) return NULL;

  /* Go over each element of the list and count how many characters there
     are in it (add one for the space with the next). */
  for(tmp=list; tmp!=NULL; tmp=tmp->next)
    {
      /* Count the characters. If we have a SPACE, we need to add an extra
         count for the back slash. */
      c=tmp->v;
      do {++bsize; if(*c==delimiter) ++bsize;} while(*(++c)!='\0');
      ++bsize; /* For the extra space between characters. */
    }

  /* Allocate the necessary space and write all the strings inside of it,
     (while also commenting the space characters). */
  out=gal_pointer_allocate(GAL_TYPE_STRING, bsize, 0, __func__, "out");
  o=out;
  for(tmp=list; tmp!=NULL; tmp=tmp->next)
    {
      c=tmp->v;
      do {if(*c==delimiter) *o++='\\'; *o++=*c;} while(*(++c)!='\0');
      if(tmp->next) *o++=delimiter;
    }
  *o='\0';

  return out;
}



















/****************************************************************
 *****************            int            ********************
 ****************************************************************/
void
gal_list_i32_add(gal_list_i32_t **list, int32_t value)
{
  gal_list_i32_t *newnode;

  errno=0;
  newnode=malloc(sizeof *newnode);
  if(newnode==NULL)
    error(EXIT_FAILURE, errno, "%s: allocating new node", __func__);

  newnode->v=value;
  newnode->next=*list;
  *list=newnode;
}





int32_t
gal_list_i32_pop(gal_list_i32_t **list)
{
  gal_list_i32_t *tmp;
  int out=GAL_BLANK_INT32;

  if(*list)
    {
      tmp=*list;
      out=tmp->v;
      *list=tmp->next;
      free(tmp);
    }
  return out;
}





size_t
gal_list_i32_number(gal_list_i32_t *list)
{
  size_t num=0;
  gal_list_i32_t *tmp;
  for(tmp=list;tmp!=NULL;tmp=tmp->next)
    ++num;
  return num;
}





gal_list_i32_t *
gal_list_i32_last(gal_list_i32_t *list)
{
  if(list)
    {
      while(list->next!=NULL) list=list->next;
      return list;
    }
  else return NULL;
}





void
gal_list_i32_print(gal_list_i32_t *list)
{
  gal_list_i32_t *tmp;
  for(tmp=list; tmp!=NULL; tmp=tmp->next)
    printf("%"PRId32"\n", tmp->v);
}





void
gal_list_i32_reverse(gal_list_i32_t **list)
{
  int thisnum;
  gal_list_i32_t *correctorder=NULL;

  if( *list && (*list)->next )
    {
      while(*list!=NULL)
        {
          thisnum=gal_list_i32_pop(list);
          gal_list_i32_add(&correctorder, thisnum);
        }
      *list=correctorder;
    }
}





int32_t *
gal_list_i32_to_array(gal_list_i32_t *list, int reverse, size_t *num)
{
  size_t i;
  int32_t *out=NULL;
  gal_list_i32_t *tmp;

  *num=gal_list_i32_number(list);

  if(*num)
    {
      out=gal_pointer_allocate(GAL_TYPE_SIZE_T, *num, 0, __func__, "out");

      i = reverse ? *num-1: 0;
      if(reverse)
        for(tmp=list;tmp!=NULL;tmp=tmp->next)
          out[i--]=tmp->v;
      else
        for(tmp=list;tmp!=NULL;tmp=tmp->next)
          out[i++]=tmp->v;
    }

  return out;
}





void
gal_list_i32_free(gal_list_i32_t *list)
{
  gal_list_i32_t *tmp, *ttmp;
  tmp=list;
  while(tmp!=NULL)
    {
      ttmp=tmp->next;
      free(tmp);
      tmp=ttmp;
    }
}




















/****************************************************************
 *****************           size_t          ********************
 ****************************************************************/
void
gal_list_sizet_add(gal_list_sizet_t **list, size_t value)
{
  gal_list_sizet_t *newnode;

  errno=0;
  newnode=malloc(sizeof *newnode);
  if(newnode==NULL)
    error(EXIT_FAILURE, errno, "%s: allocating new node", __func__);

  newnode->v=value;
  newnode->next=*list;
  *list=newnode;
}





size_t
gal_list_sizet_pop(gal_list_sizet_t **list)
{
  gal_list_sizet_t *tmp;
  size_t out=GAL_BLANK_SIZE_T;

  if(list)
    {
      tmp=*list;
      out=tmp->v;
      *list=tmp->next;
      free(tmp);
    }
  return out;
}





size_t
gal_list_sizet_number(gal_list_sizet_t *list)
{
  size_t num=0;
  gal_list_sizet_t *tmp;
  for(tmp=list;tmp!=NULL;tmp=tmp->next)
    ++num;
  return num;
}





gal_list_sizet_t *
gal_list_sizet_last(gal_list_sizet_t *list)
{
  if(list)
    {
      while(list->next!=NULL) list=list->next;
      return list;
    }
  else return NULL;
}





void
gal_list_sizet_print(gal_list_sizet_t *list)
{
  gal_list_sizet_t *tmp;
  for(tmp=list;tmp!=NULL;tmp=tmp->next)
    printf("%zu\n", tmp->v);
  return;
}





void
gal_list_sizet_reverse(gal_list_sizet_t **list)
{
  size_t thisnum;
  gal_list_sizet_t *correctorder=NULL;

  if( *list && (*list)->next )
    {
      while(*list!=NULL)
        {
          thisnum=gal_list_sizet_pop(list);
          gal_list_sizet_add(&correctorder, thisnum);
        }
      *list=correctorder;
    }
}





size_t *
gal_list_sizet_to_array(gal_list_sizet_t *list, int reverse, size_t *num)
{
  size_t i, *out=NULL;
  gal_list_sizet_t *tmp;

  *num=gal_list_sizet_number(list);

  if(*num)
    {
      out=gal_pointer_allocate(GAL_TYPE_SIZE_T, *num, 0, __func__, "out");

      i = reverse ? *num-1: 0;
      if(reverse)
        for(tmp=list;tmp!=NULL;tmp=tmp->next)
          out[i--]=tmp->v;
      else
        for(tmp=list;tmp!=NULL;tmp=tmp->next)
          out[i++]=tmp->v;
    }

  return out;
}





void
gal_list_sizet_free(gal_list_sizet_t *list)
{
  gal_list_sizet_t *tmp, *ttmp;
  tmp=list;
  while(tmp!=NULL)
    {
      ttmp=tmp->next;
      free(tmp);
      tmp=ttmp;
    }
}




















/****************************************************************
 *****************            Float          ********************
 ****************************************************************/
void
gal_list_f32_add(gal_list_f32_t **list, float value)
{
  struct gal_list_f32_t *newnode;

  errno=0;
  newnode=malloc(sizeof *newnode);
  if(newnode==NULL)
    error(EXIT_FAILURE, errno, "%s: allocating new node", __func__);

  newnode->v=value;
  newnode->next=*list;
  *list=newnode;
}





float
gal_list_f32_pop(gal_list_f32_t **list)
{
  float out=NAN;
  gal_list_f32_t *tmp;

  if(*list)
    {
      tmp=*list;
      out=tmp->v;
      *list=tmp->next;
      free(tmp);
    }
  return out;
}





size_t
gal_list_f32_number(gal_list_f32_t *list)
{
  size_t num=0;
  gal_list_f32_t *tmp;
  for(tmp=list;tmp!=NULL;tmp=tmp->next)
    ++num;
  return num;
}





gal_list_f32_t *
gal_list_f32_last(gal_list_f32_t *list)
{
  if(list)
    {
      while(list->next!=NULL) list=list->next;
      return list;
    }
  else return NULL;
}





void
gal_list_f32_reverse(gal_list_f32_t **list)
{
  float thisnum;
  gal_list_f32_t *correctorder=NULL;

  if( *list && (*list)->next )
    {
      while(*list!=NULL)
        {
          thisnum=gal_list_f32_pop(list);
          gal_list_f32_add(&correctorder, thisnum);
        }
      *list=correctorder;
    }
}





void
gal_list_f32_print(gal_list_f32_t *list)
{
  gal_list_f32_t *tmp;
  for(tmp=list;tmp!=NULL;tmp=tmp->next)
    printf("%f\n", tmp->v);
  return;
}





float *
gal_list_f32_to_array(gal_list_f32_t *list, int reverse, size_t *num)
{
  size_t i;
  float *out=NULL;
  gal_list_f32_t *tmp;

  /* Find the number of elements: */
  *num=gal_list_f32_number(list);

  /* If there is actually anything in the list, then allocate the array and
     fill it in. */
  if(*num)
    {
      /* Allocate the space: */
      out=gal_pointer_allocate(GAL_TYPE_FLOAT32, *num, 0, __func__, "out");

      /* Fill in the array. */
      i = reverse ? *num-1: 0;
      if(reverse)
        for(tmp=list;tmp!=NULL;tmp=tmp->next)
          out[i--]=tmp->v;
      else
        for(tmp=list;tmp!=NULL;tmp=tmp->next)
          out[i++]=tmp->v;
    }

  /* Return the created array. */
  return out;
}





void
gal_list_f32_free(gal_list_f32_t *list)
{
  gal_list_f32_t *tmp, *ttmp;
  tmp=list;
  while(tmp!=NULL)
    {
      ttmp=tmp->next;
      free(tmp);
      tmp=ttmp;
    }
}




















/****************************************************************
 *****************          Double           ********************
 ****************************************************************/
void
gal_list_f64_add(gal_list_f64_t **list, double value)
{
  gal_list_f64_t *newnode;

  errno=0;
  newnode=malloc(sizeof *newnode);
  if(newnode==NULL)
    error(EXIT_FAILURE, errno, "%s: allocating new node", __func__);

  newnode->v=value;
  newnode->next=*list;
  *list=newnode;
}





double
gal_list_f64_pop(gal_list_f64_t **list)
{
  double out=NAN;
  gal_list_f64_t *tmp;

  if(*list)
    {
      tmp=*list;
      out=tmp->v;
      *list=tmp->next;
      free(tmp);
    }
  return out;
}





size_t
gal_list_f64_number(gal_list_f64_t *list)
{
  size_t num=0;
  gal_list_f64_t *tmp;
  for(tmp=list;tmp!=NULL;tmp=tmp->next)
    ++num;
  return num;
}





gal_list_f64_t *
gal_list_f64_last(gal_list_f64_t *list)
{
  if(list)
    {
      while(list->next!=NULL) list=list->next;
      return list;
    }
  else return NULL;
}





void
gal_list_f64_print(gal_list_f64_t *list)
{
  gal_list_f64_t *tmp;
  for(tmp=list;tmp!=NULL;tmp=tmp->next)
    printf("%f\n", tmp->v);
  return;
}





void
gal_list_f64_reverse(gal_list_f64_t **list)
{
  double thisvalue;
  gal_list_f64_t *correctorder=NULL;

  /* Only do the reversal if there is more than one element. */
  if( *list && (*list)->next )
    {
      while(*list!=NULL)
        {
          thisvalue=gal_list_f64_pop(list);
          gal_list_f64_add(&correctorder, thisvalue);
        }
      *list=correctorder;
    }
}




double *
gal_list_f64_to_array(gal_list_f64_t *list, int reverse, size_t *num)
{
  size_t i;
  double *out=NULL;
  gal_list_f64_t *tmp;

  /* Find the number of elements: */
  *num=gal_list_f64_number(list);

  /* If there is actually anything in the list, then allocate the array and
     fill it in. */
  if(*num)
    {
      /* Allocate the space: */
      out=gal_pointer_allocate(GAL_TYPE_FLOAT64, *num, 0, __func__, "out");

      /* Fill in the array. */
      i = reverse ? *num-1: 0;
      if(reverse)
        for(tmp=list;tmp!=NULL;tmp=tmp->next)
          out[i--]=tmp->v;
      else
        for(tmp=list;tmp!=NULL;tmp=tmp->next)
          out[i++]=tmp->v;
    }

  /* Return the created array. */
  return out;
}





/* Copy a list of float64 to a 1D dataset of the desired type. */
gal_data_t *
gal_list_f64_to_data(gal_list_f64_t *list, uint8_t type,
                     size_t minmapsize, int quietmmap)
{
  double *d;
  gal_data_t *out;
  size_t num, one=1;

  /* In if the list is empty, return a dataset with no elements. */
  if(list==NULL)
    {
      /* It is not possible to allocate a dataset with a size of 0 along
         any dimension (in C it's possible, but conceptually it isn't). So,
         we'll allocate space for one element, then free it. */
      out=gal_data_alloc(NULL, type, 1, &one, NULL, 0,
                         minmapsize, quietmmap, NULL, NULL, NULL);
      out->size=out->dsize[0]=0;
      free(out->array);
      out->array=NULL;
      return out;
    }

  /* Convert the list to an array, put it into a dataset. */
  d=gal_list_f64_to_array(list, 0, &num);
  out=gal_data_alloc(d, GAL_TYPE_FLOAT64, 1, &num, NULL, 0, minmapsize,
                     quietmmap, NULL, NULL, NULL);

  /* Copy to desired type and return. */
  out=gal_data_copy_to_new_type_free(out, type);
  return out;
}





void
gal_list_f64_free(gal_list_f64_t *list)
{
  gal_list_f64_t *tmp, *ttmp;
  tmp=list;
  while(tmp!=NULL)
    {
      ttmp=tmp->next;
      free(tmp);
      tmp=ttmp;
    }
}



















/****************************************************************
 *****************          void *           ********************
 ****************************************************************/
void
gal_list_void_add(gal_list_void_t **list, void *value)
{
  gal_list_void_t *newnode;

  errno=0;
  newnode=malloc(sizeof *newnode);
  if(newnode==NULL)
    error(EXIT_FAILURE, errno, "%s: allocating new node", __func__);

  newnode->v=value;
  newnode->next=*list;
  *list=newnode;
}





void *
gal_list_void_pop(gal_list_void_t **list)
{
  void *out=NULL;
  gal_list_void_t *tmp;

  if(*list)
    {
      tmp=*list;
      out=tmp->v;
      *list=tmp->next;
      free(tmp);
    }
  return out;
}





size_t
gal_list_void_number(gal_list_void_t *list)
{
  size_t num=0;
  gal_list_void_t *tmp;
  for(tmp=list;tmp!=NULL;tmp=tmp->next)
    ++num;
  return num;
}





gal_list_void_t *
gal_list_void_last(gal_list_void_t *list)
{
  if(list)
    {
      while(list->next!=NULL) list=list->next;
      return list;
    }
  else return NULL;
}





void
gal_list_void_reverse(gal_list_void_t **list)
{
  void *thisptr;
  gal_list_void_t *correctorder=NULL;

  if( *list && (*list)->next )
    {
      while(*list!=NULL)
        {
          thisptr=gal_list_void_pop(list);
          gal_list_void_add(&correctorder, thisptr);
        }
      *list=correctorder;
    }
}





void
gal_list_void_free(gal_list_void_t *list, int freevalue)
{
  gal_list_void_t *tmp=list, *ttmp;
  while(tmp!=NULL)
    {
      if(freevalue) free(tmp->v);
      ttmp=tmp->next;
      free(tmp);
      tmp=ttmp;
    }
}




















/****************************************************************
 ****************       Ordered size_t       ********************
 ****************************************************************/
/* We want to put the nodes in order based on the 'tosort' value of
each node. The top element should always have the smallest radius. */
void
gal_list_osizet_add(gal_list_osizet_t **list,
                    size_t value, float tosort)
{
  gal_list_osizet_t *newnode, *tmp=*list, *prev=NULL;

  errno=0;
  newnode=malloc(sizeof *newnode);
  if(newnode==NULL)
    error(EXIT_FAILURE, errno, "%s: allocating new node", __func__);

  newnode->v=value;
  newnode->s=tosort;

  /* *list points to the smallest value in the queue! */
  while(tmp!=NULL)
    {
      if(tosort<tmp->s) break;
      /* No need for else, it will only come here if the condition
         above is not satisfied. */
      prev=tmp;
      tmp=tmp->next;
    }

  if(tmp==NULL)      /* This is the largest value so far. */
    {                /* '*list' only changes if it is NULL. */
      newnode->next=NULL;
      if(prev) prev->next=newnode;   /* 'prev' is not NULL! */
      else     *list=newnode;        /* Only for initial node. */
    }
  else
    {
      if(prev) prev->next=newnode;
      else     *list=newnode;        /* 'tosort' is smaller than all. */
      newnode->next=tmp;
    }
}





/* Note that the popped element is the smallest! */
size_t
gal_list_osizet_pop(gal_list_osizet_t **list, float *sortvalue)
{
  size_t value;
  gal_list_osizet_t *tmp=*list;

  if(*list)
    {
      value=tmp->v;
      *sortvalue=tmp->s;
      *list=tmp->next;
      free(tmp);
    }
  else
    {
      value=GAL_BLANK_SIZE_T;
      *sortvalue=NAN;
    }

  return value;
}





/* Add the elements of an gal_list_osll to a gal_list_sll. */
void
gal_list_osizet_to_sizet_free(gal_list_osizet_t *in,
                              gal_list_sizet_t **out)
{
  gal_list_osizet_t *tmp;
  while(in!=NULL)
    {
      tmp=in->next;
      gal_list_sizet_add(out, in->v);
      free(in);
      in=tmp;
    }
}




















/****************************************************************
 ******************   Two way, Ordered SLL   ********************
 *****************           size_t          ********************
 ****************************************************************/
/* Doubly-linked ordered size_t list can be visualized like this:

            largest pointer
            |
   NULL <-- (v0,s0) <--> (v1,s1) <--> ... (vn,sn) --> NULL
                                          |
                           smallest pointer

   Where s(n)>s(n+1) for all n.
*/
/* Very similar to Ordered SLL, but now it is two way. */
void
gal_list_dosizet_add(gal_list_dosizet_t **largest,
                     gal_list_dosizet_t **smallest, size_t value, float tosort)
{
  gal_list_dosizet_t *newnode, *tmp=*largest;

  errno=0;
  newnode=malloc(sizeof *newnode);
  if(newnode==NULL)
    error(EXIT_FAILURE, errno, "%s: allocating new node", __func__);

  newnode->v=value;
  newnode->s=tosort;
  newnode->prev=NULL;

  while(tmp!=NULL)
    {
      if(tosort >= tmp->s) break;
      /* No need for else, it will only come here if the condition
         above is not satisfied. */
      newnode->prev=tmp;
      tmp=tmp->next;
    }

  if(tmp==NULL)      /* This is the smallest value so far.     */
    {                /* '*largest' only changes if it is NULL. */
      newnode->next=NULL;
      *smallest=newnode;
      if(newnode->prev)         /* 'prev' is not NULL! */
        newnode->prev->next=newnode;
      else                      /* 'prev is NULL, Only first. */
        *largest=newnode;
    }
  else
    {
      if(newnode->prev)
        {
          newnode->prev->next->prev=newnode;
          newnode->prev->next=newnode;
        }
      else
        {
          (*largest)->prev=newnode;
          *largest=newnode;       /* 'tosort' is larger than all. */
        }
      newnode->next=tmp;
    }
}





/* Note that start has to be initialized. */
size_t
gal_list_dosizet_pop_smallest(gal_list_dosizet_t **largest,
                              gal_list_dosizet_t **smallest, float *tosort)
{
  size_t value;
  gal_list_dosizet_t *tmp=*smallest;

  if(*smallest)
    {
      value=tmp->v;
      *tosort=tmp->s;

      *smallest=tmp->prev;
      free(tmp);
      if(*smallest)
        (*smallest)->next=NULL;
      else
        *largest=NULL;
    }
  else
    {
      /* If 'smallest' is NULL, 'largest' should also be NULL. */
      if(*largest)
        error(EXIT_FAILURE, 0, "%s: 'largest' and 'smallest' pointers must "
              "both be non-NULL or both be NULL. However, in this call, "
              "'smallest' was NULL while 'largest' isn't NULL", __func__);
      value=GAL_BLANK_SIZE_T;
      *tosort=NAN;
    }

  /*printf("Popped v: %zu, s: %f\n", *value, *tosort);*/

  return value;
}





void
gal_list_dosizet_print(gal_list_dosizet_t *largest,
                       gal_list_dosizet_t *smallest)
{
  size_t counter=1;   /* We are not counting array elements :-D ! */
  while(largest!=NULL)
    {
      printf("\t%-5zu (%zu, %.4f) \n", counter++,
             largest->v, largest->s);
      largest=largest->next;
      printf("\t\t\t\t(%zu, %.4f)\n", smallest->v, smallest->s);
      smallest=smallest->prev;
    }
  printf("\n");
}





void
gal_list_dosizet_to_sizet(gal_list_dosizet_t *in, gal_list_sizet_t **out)
{
  gal_list_dosizet_t *tmp;
  while(in!=NULL)
    {
      tmp=in->next;
      gal_list_sizet_add(out, in->v);
      free(in);
      in=tmp;
    }
}





void
gal_list_dosizet_free(gal_list_dosizet_t *largest)
{
  gal_list_dosizet_t *tmp;
  while(largest!=NULL)
    {
      tmp=largest->next;
      free(largest);
      largest=tmp;
    }
}




















/*********************************************************************/
/*************    Data structure as a linked list   ******************/
/*********************************************************************/
/* Add a new data structure to the top of an existing linked list of data
   structures. Note that if the new node is its self a list, all its nodes
   will be added to the list. */
void
gal_list_data_add(gal_data_t **list, gal_data_t *newnode)
{
  gal_data_t *tmp=newnode, *toadd;

  /* Check if newnode is itself a list or not. */
  if(newnode->next)
    {
      /* Go onto the last node in newnode's existing list. */
      while(tmp->next) tmp=tmp->next;

      /* Set the last node as the node to add to the list. */
      toadd=tmp;
    }
  else
    /* Its not a list, so just set it to 'toadd'. */
    toadd=newnode;

  /* Set the next element of toadd and update what list points to. */
  toadd->next=*list;
  *list=newnode;
}





void
gal_list_data_add_alloc(gal_data_t **list, void *array, uint8_t type,
                        size_t ndim, size_t *dsize, struct wcsprm *wcs,
                        int clear, size_t minmapsize, int quietmmap,
                        char *name, char *unit, char *comment)
{
  gal_data_t *newnode;

  /* Put all the input information into a new data structure node. */
  newnode=gal_data_alloc(array, type, ndim, dsize, wcs, clear,
                         minmapsize, quietmmap, name, unit, comment);

  /* Add the new node to the list. */
  gal_list_data_add(list, newnode);
}





gal_data_t *
gal_list_data_pop(gal_data_t **list)
{
  gal_data_t *out=NULL;

  /* If list is not empty. */
  if(*list)
    {
      /* Keep the top pointer. */
      out=*list;

      /* Move the list pointer to the next node. */
      *list=out->next;

      /* Set the next poitner of the out pointer to NULL so it isn't
         interpretted as a list any more. */
      out->next=NULL;
    }
  return out;
}





/* Remove one node from the list. */
void
gal_list_data_remove(gal_data_t **list, gal_data_t *node)
{
  int found=0;
  gal_data_t *tmp, *prev=*list;

  /* If this is an empty list, just ignore it. */
  if(*list==NULL || node==NULL) return;

  /* If the requested node is first. */
  if(node==*list) { found=1; *list=(*list)->next; }
  else
    {
      /* Parse the list, while keeping track of the previous. */
      for(tmp=(*list)->next;tmp!=NULL;tmp=tmp->next)
        {
          /* This is the desired node, remove it from the list. */
          if(tmp==node) { found=1; prev->next=tmp->next; break; }

          /* Set the current pointer to the "prev" of the next. */
          prev=tmp;
        }
    }

  /* If 'node' has been identified, fully detach it from the list. */
  if(found) node->next=NULL;
}





/* From the input list of datasets, return the first one that has a name
   equal to the input string 'name'. */
gal_data_t *
gal_list_data_select_by_name(gal_data_t *list, char *name)
{
  gal_data_t *tmp;

  /* Parse the list and return the desired element. */
  for(tmp=list;tmp!=NULL;tmp=tmp->next)
    if(tmp->name && strcmp(tmp->name,name)==0 )
      return tmp;

  /* If control reaches here, no such name could be found. */
  return NULL;
}





/* Select a dataset from a list from its idenfier (name or counter in a
   string). */
gal_data_t *
gal_list_data_select_by_id(gal_data_t *table, char *idstr, size_t *index)
{
  char *tailptr;
  gal_data_t *tmp, *out=NULL;
  size_t i, oind=GAL_BLANK_SIZE_T, colind;

  /* If given string identifier ('idstr') is a number, then 'strtol' will
     set 'tailptr' to '\0'. Otherwise, we will need to check the existing
     column names in the table. */
  colind=strtol(idstr, &tailptr, 10);
  if(tailptr[0]=='\0') /* ID is a number. */
    {
      /* Parse the list and return the desired column. Note that the column
         counter in the ID is assumed to be from 1, but the output "index"
         should start from 0. If the requested counter is larger than the
         input's number of columns, the output will automatically be NULL
         (it has been initialized). */
      i=0;
      for(tmp=table;tmp!=NULL;tmp=tmp->next)
        { ++i; if(i==colind) { oind=i-1; out=tmp; break;} }
    }
  else /* ID is string; parse the names in the table. */
    {
      /* Parse the table and if the name exists, return it. */
      colind=i=0;
      for(tmp=table;tmp!=NULL;tmp=tmp->next)
        {
          ++i;
          if( !strcmp(idstr, tmp->name) ) { oind=i-1; out=tmp; break; }
        }
    }

  /* For a check.
  printf("%s: %s is column index %zu\n", __func__, tmp->name, oind);
  */

  /* Fill the 'index' poiter (if it is not NULL!) and return 'out'. */
  if(index) *index=oind;
  return out;
}





void
gal_list_data_reverse(gal_data_t **list)
{
  gal_data_t *popped, *in=*list, *reversed=NULL;

  /* Only do the job if the list is not NULL and has more than one node. */
  if( in && in->next )
    {
      while(in!=NULL)
        {
          popped=gal_list_data_pop(&in);
          gal_list_data_add(&reversed, popped);
        }
      *list=reversed;
    }
}





gal_data_t **
gal_list_data_to_array_ptr(gal_data_t *list, size_t *num)
{
  size_t i, n;
  gal_data_t *tmp, **out;

  /* Count how many columns are necessary. */
  n=*num=gal_list_data_number(list);

  /* Allocate space for the array. */
  errno=0;
  out=malloc(n * sizeof *out);
  if(out==NULL)
    error(EXIT_FAILURE, 0, "%s: couldn't allocate %zu bytes", __func__,
          n * sizeof *out);

  /* Fill up the array with the pointers and return. */
  i=0;
  for(tmp=list;tmp!=NULL;tmp=tmp->next) out[i++]=tmp;
  return out;
}





size_t
gal_list_data_number(gal_data_t *list)
{
  size_t num=0;
  while(list!=NULL)
    {
      ++num;
      list=list->next;
    }
  return num;
}





gal_data_t *
gal_list_data_last(gal_data_t *list)
{
  if(list)
    {
      while(list->next!=NULL) list=list->next;
      return list;
    }
  else return NULL;
}




void
gal_list_data_free(gal_data_t *list)
{
  struct gal_data_t *tmp;
  while(list!=NULL)
    {
      tmp=list->next;
      gal_data_free(list);
      list=tmp;
    }
}
