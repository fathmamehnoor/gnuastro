/*********************************************************************
Extensions to GNU Make for working with FITS files.
This is part of GNU Astronomy Utilities (Gnuastro) package.

Original author:
     Mohammad Akhlaghi <mohammad@akhlaghi.org>
Contributing author(s):
Copyright (C) 2022-2024 Free Software Foundation, Inc.

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

#include <gnumake.h>

#include <gnuastro/txt.h>
#include <gnuastro/pointer.h>

#include <gnuastro-internal/options.h>
#include <gnuastro-internal/checkset.h>





/* Necessary for GNU Make. */
int plugin_is_GPL_compatible=1;





/* Names of the separate functions. */
#define MAKEPLUGIN_FUNC_PREFIX "ast"

/* Basic text functions */
static char *text_prev=MAKEPLUGIN_FUNC_PREFIX"-text-prev";
static char *text_to_upper=MAKEPLUGIN_FUNC_PREFIX"-text-to-upper";
static char *text_to_lower=MAKEPLUGIN_FUNC_PREFIX"-text-to-lower";
static char *text_prev_batch=MAKEPLUGIN_FUNC_PREFIX"-text-prev-batch";
static char *text_prev_batch_by_ram=MAKEPLUGIN_FUNC_PREFIX"-text-prev-batch-by-ram";
static char *text_contains_name=MAKEPLUGIN_FUNC_PREFIX"-text-contains";
static char *text_not_contains_name=MAKEPLUGIN_FUNC_PREFIX"-text-not-contains";

/* Gnuastro analysis functions */
static char *version_is_name=MAKEPLUGIN_FUNC_PREFIX"-version-is";
static char *fits_with_keyvalue_name=MAKEPLUGIN_FUNC_PREFIX"-fits-with-keyvalue";
static char *fits_unique_keyvalues_name=MAKEPLUGIN_FUNC_PREFIX"-fits-unique-keyvalues";










/**********************************************************************/
/***************          Configuration function        ***************/
/**********************************************************************/
static char *
makeplugin_version_is(const char *caller, unsigned int argc, char **argv)
{
  int check=0;
  char *out=NULL;
  char *version=gal_txt_trim_space(argv[0]);

  /* If the version matches, set the value of 'check'. */
  if( version && !strcmp(PACKAGE_VERSION, version) ) check=1;

  /* Write the value into the 'out' pointer. */
  if( asprintf(&out, "%d", check)<0 )
    error(EXIT_FAILURE, 0, "%s: couldn't allocate output string",
          __func__);

  /* Return the output string. */
  return out;
}




















/**********************************************************************/
/***************             Text functions             ***************/
/**********************************************************************/

/* Base function that is used for both the contains and not-contains
   functions. */
static char *
makeplugin_text_contains_base(char **argv, int has1_not0)
{
  char *out=NULL;
  gal_list_str_t *tmp, *outlist=NULL;
  char *match=argv[0]; /* No trimming the white space before/after, as in */
  gal_list_str_t *strings=gal_list_str_extract(argv[1]); /* Make itself.  */

  /* Parse the input strings and find the ones that match. */
  for(tmp=strings; tmp!=NULL; tmp=tmp->next)
    if( gal_txt_contains_string(tmp->v, match)==has1_not0 )
      gal_list_str_add(&outlist, tmp->v, 0);

  /* Write the list into one string, but first reverse it so it has the
     same order as the input. */
  gal_list_str_reverse(&outlist);
  out=gal_list_str_cat(outlist, ' ');

  /* Clean up and return. */
  gal_list_str_free(strings, 1);
  gal_list_str_free(outlist, 0); /* We didn't allocate these. */
  return out;
}





/* Return any of the input strings that contain the given string. It takes
   two arguments:
      0. String to check.
      1. List of text. */
static char *
makeplugin_text_contains(const char *caller, unsigned int argc,
                         char **argv)
{
  return makeplugin_text_contains_base(argv, 1);
}





/* Return any of the input strings that contain the given string. It takes
   two arguments:
      0. String to check.
      1. List of text. */
static char *
makeplugin_text_not_contains(const char *caller, unsigned int argc,
                             char **argv)
{
  return makeplugin_text_contains_base(argv, 0);
}





/* Convert input string to upper-case. */
static char *
makeplugin_text_to_upper(const char *caller, unsigned int argc,
                         char **argv)
{
  char *out;
  gal_checkset_allocate_copy(argv[0], &out);
  gal_checkset_string_case_change(out, 1);
  return out;
}





/* Convert input string to upper-case. */
static char *
makeplugin_text_to_lower(const char *caller, unsigned int argc,
                         char **argv)
{
  char *out;
  gal_checkset_allocate_copy(argv[0], &out);
  gal_checkset_string_case_change(out, 0);
  return out;
}





/* Return the previous word in the given list. */
static char *
makeplugin_text_prev(const char *caller, unsigned int argc, char **argv)
{
  int found=0;
  char *prev=NULL, *target=argv[0];
  gal_list_str_t *tmp, *list=gal_list_str_extract(argv[1]);

  /* Parse the input list. */
  for(tmp=list; tmp!=NULL; tmp=tmp->next)
    {
      if( strcmp(tmp->v,target) ) prev=tmp->v; /* Not equal. */
      else {found=1; break;}                   /* Equal.     */
    }

  /* Return the output. */
  return found?prev:NULL;
}





/* Given one of the words of the input list, this function will return a
   string containing the previous batch of words. */
static char *
makeplugin_text_prev_batch_work(char *target, size_t num_in_batch,
                                char *list)
{
  int is_first_batch=1;
  size_t anum=0, starti, endi, outlen;
  char *startend[4]={NULL, NULL, NULL, NULL};
  char *cp, *token, *saveptr=NULL, *out=NULL, *delimiters=" ";

  /* Parse the line to find the desired element, but first copy the input
     list into a new editable space with 'strdupa'. */
  gal_checkset_allocate_copy(list, &cp);
  token=strtok_r(cp, delimiters, &saveptr);
  do
    {
      /* For the first num_in_batch elements, we don't should not set the
         first two pointers of 'startend'. Startend contains the following
         four pointers:

           startend[0] --> FIRST token in PREVIOUS batch.
           startend[1] --> LAST  token in PREVIOUS batch.
           startend[2] --> FIRST token in THIS     batch.
           startend[3] --> LAST  token in THIS     batch.

         First, let's check if we at the start of a batch: if so, all the
         elements of 'startend' need to be reset. */
      if(anum % num_in_batch==0)
        {
          /* Only for the non-first batches: move the start/end to the
             "PREVIOUS" batch and remove the ending of current batch. */
          if(is_first_batch==0)
            {
              startend[0]=startend[2];
              startend[1]=startend[3];
            }

          /* Put this token at the start of this batch and set the end of
             this batch to be NULL. */
          startend[3]=NULL;
          startend[2]=token;
        }

      /* This is the last element of this batch, write startend[3] and
         remove the 'isfirstbatch' flag. */
      if(anum % num_in_batch == num_in_batch-1)
        { startend[3]=token; is_first_batch=0; }

      /* For a check:
      printf("%zu: %s (%zu of %zu)\n"
             "     %s\n     %s\n     %s\n     %s\n\n", anum,
             token, (anum%num_in_batch)+1, num_in_batch, startend[0],
             startend[1], startend[2], startend[3]);
      */

      /* If the target is reached, break out of the loop. */
      if( !strcmp(target, token) ) break;

      /* Go to the next token. */
      ++anum; /* Count of all tokens. */
      token=strtok_r(NULL, delimiters, &saveptr);
    }
  while(token);

  /* We need to return a non-empty output only when a previous batch
     exists.*/
  if(startend[0])
    {
      /* Find the positions of the start and end of the output string
         within the (copied) input string and from that measure the length
         of the output string. */
      starti=startend[0]-cp;
      endi=startend[1]+strlen(startend[1])-cp;
      outlen=endi-starti;

      /* Allocate the output and copy the input into it. */
      out=gal_pointer_allocate(GAL_TYPE_STRING, outlen+1, 0, __func__,
                               "out");
      memcpy(out, &list[starti], outlen);
      out[outlen]='\0';
    }
  else out=NULL;

  /* Clean up and return. */
  free(cp);
  return out;
}





/* Return the previous word in the given list. */
static char *
makeplugin_text_prev_batch(const char *caller, unsigned int argc,
                           char **argv)
{
  size_t num;
  void *nptr;
  char *target=argv[0], *list=argv[2];

  /* Interpret the number. */
  nptr=&num;
  if( gal_type_from_string(&nptr, argv[1], GAL_TYPE_SIZE_T) )
    error(EXIT_SUCCESS, 0, "'%s' could not be read as an "
          "unsigned integer", argv[1]);

  /* Generate the outputs.*/
  return makeplugin_text_prev_batch_work(target, num, list);
}





/* Return the previous elements in the list based on the available RAM and
   the amount of RAM each invocation needs (in Gigabytes of normal decimal
   1000s base, not 1024s). To get the amount of RAM of each invocation, you
   can use the command below (which will return the maximum necessary RAM
   in Kilobytes).

   /usr/bin/time --format=%M command ....

   To convert this to Gigabytes, you can use this command (just replace the
   number):

   echo "2374764" | awk '{print $1*1e3/1e9}'

   Afterwards, try to round it upwards (so some RAM is always left for the
   OS).
*/
static char *
makeplugin_text_prev_batch_by_ram(const char *caller, unsigned int argc,
                                  char **argv)
{
  void *nptr;
  float needed_gb;
  char *c, *target=argv[0], *list=argv[2];
  size_t num, ram_b=gal_checkset_ram_available(1);

  /* In case the second argument (Gigabytes) is an empty string (only
     containing the C locale space characters identified with 'isspace'),
     then just return an empty string. */
  for(c=argv[1]; *c!='\0'; ++c) if(!isspace(*c)) break;
  if(*c=='\0') return NULL; /* Only when the string is only "space". */

  /* Interpret the number. */
  nptr=&needed_gb;
  if( gal_type_from_string(&nptr, argv[1], GAL_TYPE_FLOAT32) )
    error(EXIT_SUCCESS, 0, "'%s' could not be read as an "
          "unsigned integer", argv[1]);

  /* Estimate the number of words in each batch (to be run in parallel if
     this function is used in targets list) and call the final function. */
  num=(size_t)(ram_b/(needed_gb*1e9));
  return makeplugin_text_prev_batch_work(target, num, list);
}



















/**********************************************************************/
/***************             FITS functions             ***************/
/**********************************************************************/

/* Select the input files that have the requested value(s) in the requested
   keywords. */
static int
makeplugin_fits_check_input(char **argv, size_t numargs, char *name)
{
  char *c;
  size_t i;

  /* If the HDU is empty, print a warning and don't continue. */
  for(i=0;i<numargs;++i)
    {
      /* We need to skip white-space characters. */
      c=argv[i]; do if(isspace(*c)) ++c; else break; while(1);
      if(*c=='\0')
        {
          if(i>0) /* Message only necessary for first argument. */
            error(EXIT_SUCCESS, 0, "%s: argument %zu is empty",
                  name, i+1);
          return 0;
        }
    }

  /* If control reaches here, everything is good. */
  return 1;
}





/* Select files, were a certain keyword has a certain value. It takes four
   arguments:
       0. Keyword name.
       1. Keyword value(s).
       2. HDU (fixed in all files).
       3. List of files. */
static char *
makeplugin_fits_with_keyvalue(const char *caller, unsigned int argc,
                              char **argv)
{
  gal_list_str_t *outlist=NULL;
  char *name=gal_txt_trim_space(argv[0]);
  gal_list_str_t *files=NULL, *values=NULL;
  char *out, *hdu=gal_txt_trim_space(argv[2]);

  /* If any of the inputs are empty, then don't bother continuing. */
  if( makeplugin_fits_check_input(argv, 4, fits_with_keyvalue_name)==0 )
    return NULL;

  /* Extract the components in the arguments with possibly multiple
     values and find the output files. */
  files=gal_list_str_extract(argv[3]);
  values=gal_list_str_extract(argv[1]);
  outlist=gal_fits_with_keyvalue(files, hdu, name, values, NULL);

  /* Write the output string. */
  out=gal_list_str_cat(outlist, ' ');

  /* Clean up and return. */
  gal_list_str_free(files, 1);
  gal_list_str_free(values, 1);
  gal_list_str_free(outlist, 1);
  return out;
}




/* Return the unique values given to a certain keyword in many FITS
   files. It takes three arguments.
       0. Keyword name.
       1. HDU (fixed in all files).
       2. List of files. */
static char *
makeplugin_fits_unique_keyvalues(const char *caller, unsigned int argc,
                                 char **argv)
{
  gal_list_str_t *files=NULL;
  gal_list_str_t *outlist=NULL;
  char *name=gal_txt_trim_space(argv[0]);
  char *out, *hdu=gal_txt_trim_space(argv[1]);

  /* If any of the inputs are empty, then don't bother continuing. */
  if( makeplugin_fits_check_input(argv, 3, fits_unique_keyvalues_name)==0 )
    return NULL;

  /* Extract the components in the arguments with possibly multiple
     values and find the output files. */
  files=gal_list_str_extract(argv[2]);
  outlist=gal_fits_unique_keyvalues(files, hdu, name, NULL);

  /* Write the output value. */
  out=gal_list_str_cat(outlist, ' ');

  /* Clean up and return. */
  gal_list_str_free(files, 1);
  gal_list_str_free(outlist, 1);
  return out;
}




















/**********************************************************************/
/********          High-level interface with Make             *********/
/**********************************************************************/
int
libgnuastro_make_gmk_setup()
{
  /* ------------ Basic useful text functions ------------ */
  /* Return the input strings that contain the given string. */
  gmk_add_function(text_contains_name, makeplugin_text_contains,
                   2, 2, GMK_FUNC_DEFAULT);

  /* Return the input strings that DON'T contain the given string. */
  gmk_add_function(text_not_contains_name, makeplugin_text_not_contains,
                   2, 2, GMK_FUNC_DEFAULT);

  /* Convert input sting into upper-case. */
  gmk_add_function(text_to_upper, makeplugin_text_to_upper,
                   1, 1, GMK_FUNC_DEFAULT);

  /* Convert input string to lower-case. */
  gmk_add_function(text_to_lower, makeplugin_text_to_lower,
                   1, 1, GMK_FUNC_DEFAULT);

  /* Select previous item in list*/
  gmk_add_function(text_prev, makeplugin_text_prev, 2, 2,
                   GMK_FUNC_DEFAULT);

  /* Select batch of previous 'num' elements in list. */
  gmk_add_function(text_prev_batch, makeplugin_text_prev_batch,
                   3, 3, GMK_FUNC_DEFAULT);

  /* Select batch  */
  gmk_add_function(text_prev_batch_by_ram,
                   makeplugin_text_prev_batch_by_ram,
                   3, 3, GMK_FUNC_DEFAULT);





  /* ------------ Gnuastro related functions ------------ */
  /* Return 1 if Gnuastro has the requested version. */
  gmk_add_function(version_is_name, makeplugin_version_is,
                   1, 1, GMK_FUNC_DEFAULT);

  /* Select files, were a certain keyword has a certain value. It takes
     four arguments. */
  gmk_add_function(fits_with_keyvalue_name, makeplugin_fits_with_keyvalue,
                   4, 4, GMK_FUNC_DEFAULT);

  /* Return the unique values given to a certain keyword in many FITS
     files. */
  gmk_add_function(fits_unique_keyvalues_name,
                   makeplugin_fits_unique_keyvalues,
                   3, 3, GMK_FUNC_DEFAULT);

  /* Everything is good, return 1 (success). */
  return 1;
}
