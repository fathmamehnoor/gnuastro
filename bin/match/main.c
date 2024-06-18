/*********************************************************************
Match - A program to match catalogs and WCS warps
Match is part of GNU Astronomy Utilities (Gnuastro) package.

Original author:
     Mohammad Akhlaghi <mohammad@akhlaghi.org>
Contributing author(s):
Copyright (C) 2017-2024 Free Software Foundation, Inc.

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
#include <stdlib.h>

#include <gnuastro-internal/timing.h>


#include "main.h"

#include "ui.h"
#include "match.h"



/* Main function */
int
main (int argc, char *argv[])
{
  struct timeval t1;
  struct matchparams p={{{0},0},0};

  /* Set they starting time. */
  time(&p.rawtime);
  gettimeofday(&t1, NULL);

  /* Read the input parameters. */
  ui_read_check_inputs_setup(argc, argv, &p);

  /* Run Match. */
  match(&p);

  /* Free all non-freed allocations. */
  ui_free_report(&p, &t1);

  /* Return successfully. */
  return EXIT_SUCCESS;
}
