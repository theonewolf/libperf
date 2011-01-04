/******************************************************************************
 * benchmark.c                                                                *
 *                                                                            *
 * This is an application that can benchmark libperf performance using the    *
 * sysbench benchmarking tool.                                                *
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

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "libperf.h"

char *const tests[][10] = {
  {"/usr/bin/sysbench", "--test=cpu", "--cpu-max-prime=20000", "run", (char *) 0},
  {"/usr/bin/sysbench", "--num-threads=64", "--test=threads", "--thread-yields=7500", "--thread-locks=8", "run", (char *) 0},
  {"/usr/bin/sysbench", "--test=mutex", "--mutex-num=4096", "--mutex-locks=300000000", "--mutex-loops=10000000", "run", (char *) 0},
  {"/usr/bin/sysbench", "--test=memory", "--memory-block-size=4K", "--memory-total-size=23G", "run", (char *) 0},
  {"/usr/bin/sysbench", "--num-threads=16", "--test=fileio", "--file-total-size=3G", "--file-test-mode=rndrw", "prepare", (char *) 0},
  {"/usr/bin/sysbench", "--num-threads=2", "--max-requests=7000", "--test=fileio", "--file-total-size=3G", "--file-test-mode=rndrw", "run", (char *) 0},
  {"/usr/bin/sysbench", "--num-threads=16", "--test=fileio", "--file-total-size=3G", "--file-test-mode=rndrw", "clean", (char *) 0}
};

int
main(int argc, char *argv[])
{
  if (argc < 2)
    fprintf(stderr, "Usage: bench <test #>\n");

  int test = atoi(argv[1]), status, i = 0;

  fprintf(stdout, "cmd[");

  while (tests[test][i] != 0)
    fprintf(stdout, " %s", tests[test][i++]);

  fprintf(stdout, " ]\n");


  pid_t pid = fork();

  struct libperf_data *pd = libperf_initialize(pid, -1);

  if (pid == 0)
  {
      execv(tests[test][0], tests[test]);
  }
  else
  {
      wait(&status);
      libperf_finalize(pd, pd);
  }

  return 0;
}
