/******************************************************************************
 * libperf_example.c                                                          *
 *                                                                            *
 * This is an example program showing how to use libperf within C code.       *
 *                                                                            *
 * libperf interfaces with the kernel performance counters subsystem          *
 * Copyright (C) 2010 Wolfgang Richter, Ekaterina Taralova, Karl Naden        *
 *                                                                            *
 * This program is free software; you can redistribute it and/or              *
 * modify it under the terms of the GNU General Public License                *
 * as published by the Free Software Foundation; either version 2             *
 * of the License, or (at your option) any later version.                     *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program; if not, write to the Free Software                *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA              *
 * 02110-1301, USA.                                                           *
 ******************************************************************************/

#include <inttypes.h>           /* for PRIu64 definition */
#include <stdint.h>             /* for uint64_t and PRIu64 */
#include <stdio.h>              /* for printf family */
#include <stdlib.h>             /* for EXIT_SUCCESS definition */
#include "libperf.h"            /* standard libperf include */

int
main(int argc, char *argv[])
{
  struct libperf_data *pd = libperf_initialize(-1, -1);                      /* init lib */

  libperf_enablecounter(pd, LIBPERF_COUNT_HW_INSTRUCTIONS);                  /* enable HW counter */

  uint64_t counter = libperf_readcounter(pd, LIBPERF_COUNT_HW_INSTRUCTIONS); /* obtain counter value */

  libperf_disablecounter(pd, LIBPERF_COUNT_HW_INSTRUCTIONS);                 /* disable HW counter */

  fprintf(stdout, "counter read: %" PRIu64 "\n", counter);                   /* printout */

  FILE *log = libperf_getlogger(pd);                                         /* get log file stream */

  fprintf(log, "custom log message\n");                                      /* print a custom log message */

  libperf_finalize(pd, 0);                                                   /* log all counter values */

  return EXIT_SUCCESS;                                                       /* success exit value */
}
