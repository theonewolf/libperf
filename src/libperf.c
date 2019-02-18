/******************************************************************************
 * libperf.c                                                                  *
 *                                                                            *
 * This is the libperf implementation.                                        *
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

#include <assert.h>
#include <fcntl.h>
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stropts.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <linux/perf_event.h>

#include "libperf.h"

#define __LIBPERF_MAX_COUNTERS 32 
#define __LIBPERF_ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

/* lib struct */
struct libperf_data
{
  int group;
  int fds[__LIBPERF_MAX_COUNTERS];
  struct perf_event_attr *attrs;
  FILE *log;
  pid_t pid;
  int cpu;
  unsigned long long wall_start;
};

/* rdclock() function */
static inline unsigned long long rdclock(void)
{
  struct timespec ts;

  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

/* stats section */
struct stats
{
  double n, mean, M2;
};

static void
update_stats(struct stats *stats, uint64_t val)
{
  double delta;

  stats->n++;
  delta = val - stats->mean;
  stats->mean += delta / stats->n;
  stats->M2 += delta * (val - stats->mean);
}

static double
avg_stats(struct stats *stats)
{
  return stats->mean;
}

/* perf_event_open syscall wrapper */
static long
sys_perf_event_open(struct perf_event_attr *hw_event,
                    pid_t pid, int cpu, int group_fd,
                    unsigned long flags)
{
  return syscall(__NR_perf_event_open, hw_event, pid, cpu,
                 group_fd, flags);
}

/* gettid syscall wrapper */
static inline pid_t
gettid()
{
  return syscall(SYS_gettid);
}

/* perf specific */
static struct perf_event_attr default_attrs[] = {

  { .type = PERF_TYPE_SOFTWARE, .config = PERF_COUNT_SW_CPU_CLOCK          },
  { .type = PERF_TYPE_SOFTWARE, .config = PERF_COUNT_SW_TASK_CLOCK         },
  { .type = PERF_TYPE_SOFTWARE, .config = PERF_COUNT_SW_CONTEXT_SWITCHES   },
  { .type = PERF_TYPE_SOFTWARE, .config = PERF_COUNT_SW_CPU_MIGRATIONS     },
  { .type = PERF_TYPE_SOFTWARE, .config = PERF_COUNT_SW_PAGE_FAULTS        },
  { .type = PERF_TYPE_SOFTWARE, .config = PERF_COUNT_SW_PAGE_FAULTS_MIN    },
  { .type = PERF_TYPE_SOFTWARE, .config = PERF_COUNT_SW_PAGE_FAULTS_MAJ    },

  { .type = PERF_TYPE_HARDWARE, .config = PERF_COUNT_HW_CPU_CYCLES         },
  { .type = PERF_TYPE_HARDWARE, .config = PERF_COUNT_HW_INSTRUCTIONS       },
  { .type = PERF_TYPE_HARDWARE, .config = PERF_COUNT_HW_CACHE_REFERENCES   },
  { .type = PERF_TYPE_HARDWARE, .config = PERF_COUNT_HW_CACHE_MISSES       },
  { .type = PERF_TYPE_HARDWARE, .config = PERF_COUNT_HW_BRANCH_INSTRUCTIONS},
  { .type = PERF_TYPE_HARDWARE, .config = PERF_COUNT_HW_BRANCH_MISSES      },
  { .type = PERF_TYPE_HARDWARE, .config = PERF_COUNT_HW_BUS_CYCLES         },

  { .type = PERF_TYPE_HW_CACHE, .config = (PERF_COUNT_HW_CACHE_L1D | (PERF_COUNT_HW_CACHE_OP_READ << 8) | (PERF_COUNT_HW_CACHE_RESULT_ACCESS << 16))},
  { .type = PERF_TYPE_HW_CACHE, .config = (PERF_COUNT_HW_CACHE_L1D | (PERF_COUNT_HW_CACHE_OP_READ << 8) | (PERF_COUNT_HW_CACHE_RESULT_MISS << 16))},
  { .type = PERF_TYPE_HW_CACHE, .config = (PERF_COUNT_HW_CACHE_L1D | (PERF_COUNT_HW_CACHE_OP_WRITE << 8) | (PERF_COUNT_HW_CACHE_RESULT_ACCESS << 16))},
  { .type = PERF_TYPE_HW_CACHE, .config = (PERF_COUNT_HW_CACHE_L1D | (PERF_COUNT_HW_CACHE_OP_WRITE << 8) | (PERF_COUNT_HW_CACHE_RESULT_MISS << 16))},
  { .type = PERF_TYPE_HW_CACHE, .config = (PERF_COUNT_HW_CACHE_L1D | (PERF_COUNT_HW_CACHE_OP_PREFETCH << 8) | (PERF_COUNT_HW_CACHE_RESULT_ACCESS << 16))},
  { .type = PERF_TYPE_HW_CACHE, .config = (PERF_COUNT_HW_CACHE_L1I | (PERF_COUNT_HW_CACHE_OP_READ << 8) | (PERF_COUNT_HW_CACHE_RESULT_ACCESS << 16))},
  { .type = PERF_TYPE_HW_CACHE, .config = (PERF_COUNT_HW_CACHE_L1I | (PERF_COUNT_HW_CACHE_OP_READ << 8) | (PERF_COUNT_HW_CACHE_RESULT_MISS << 16))},
  { .type = PERF_TYPE_HW_CACHE, .config = (PERF_COUNT_HW_CACHE_LL | (PERF_COUNT_HW_CACHE_OP_READ << 8) | (PERF_COUNT_HW_CACHE_RESULT_ACCESS << 16))},
  { .type = PERF_TYPE_HW_CACHE, .config = (PERF_COUNT_HW_CACHE_LL | (PERF_COUNT_HW_CACHE_OP_READ << 8) | (PERF_COUNT_HW_CACHE_RESULT_MISS << 16))},
  { .type = PERF_TYPE_HW_CACHE, .config = (PERF_COUNT_HW_CACHE_LL | (PERF_COUNT_HW_CACHE_OP_WRITE << 8) | (PERF_COUNT_HW_CACHE_RESULT_ACCESS << 16))},
  { .type = PERF_TYPE_HW_CACHE, .config = (PERF_COUNT_HW_CACHE_LL | (PERF_COUNT_HW_CACHE_OP_WRITE << 8) | (PERF_COUNT_HW_CACHE_RESULT_MISS << 16))},
  { .type = PERF_TYPE_HW_CACHE, .config = (PERF_COUNT_HW_CACHE_DTLB | (PERF_COUNT_HW_CACHE_OP_READ << 8) | (PERF_COUNT_HW_CACHE_RESULT_ACCESS << 16))},
  { .type = PERF_TYPE_HW_CACHE, .config = (PERF_COUNT_HW_CACHE_DTLB | (PERF_COUNT_HW_CACHE_OP_READ << 8) | (PERF_COUNT_HW_CACHE_RESULT_MISS << 16))},
  { .type = PERF_TYPE_HW_CACHE, .config = (PERF_COUNT_HW_CACHE_DTLB | (PERF_COUNT_HW_CACHE_OP_WRITE << 8) | (PERF_COUNT_HW_CACHE_RESULT_ACCESS << 16))},
  { .type = PERF_TYPE_HW_CACHE, .config = (PERF_COUNT_HW_CACHE_DTLB | (PERF_COUNT_HW_CACHE_OP_WRITE << 8) | (PERF_COUNT_HW_CACHE_RESULT_MISS << 16))},
  { .type = PERF_TYPE_HW_CACHE, .config = (PERF_COUNT_HW_CACHE_ITLB | (PERF_COUNT_HW_CACHE_OP_READ << 8) | (PERF_COUNT_HW_CACHE_RESULT_ACCESS << 16))},
  { .type = PERF_TYPE_HW_CACHE, .config = (PERF_COUNT_HW_CACHE_ITLB | (PERF_COUNT_HW_CACHE_OP_READ << 8) | (PERF_COUNT_HW_CACHE_RESULT_MISS << 16))},
  { .type = PERF_TYPE_HW_CACHE, .config = (PERF_COUNT_HW_CACHE_BPU | (PERF_COUNT_HW_CACHE_OP_READ << 8) | (PERF_COUNT_HW_CACHE_RESULT_ACCESS << 16))},
  /* { .type = PERF_TYPE_HW_CACHE, .config = (PERF_COUNT_HW_CACHE_BPU | (PERF_COUNT_HW_CACHE_OP_READ << 8) | (PERF_COUNT_HW_CACHE_RESULT_MISS << 16))}, */

};

/* thread safe */
/* sets up a set of fd's for profiling code to read from */
struct libperf_data *
libperf_initialize(pid_t pid, int cpu)
{
  int nr_counters = __LIBPERF_ARRAY_SIZE(default_attrs);

  int i;

  struct libperf_data *pd = malloc(sizeof(struct libperf_data));

  if (pd == NULL)
    {
      perror("malloc");
      exit(EXIT_FAILURE);
    }

  if (pid == -1)
    pid = gettid();

  pd->group = -1;

  for (i = 0; i < __LIBPERF_ARRAY_SIZE(pd->fds); i++)
    pd->fds[i] = -1;

  pd->pid = pid;
  pd->cpu = cpu;

  char logname[256];

  struct perf_event_attr *attrs =
    malloc(nr_counters * sizeof(struct perf_event_attr));

  if(attrs == NULL)
    {
      perror("malloc");
      exit(EXIT_FAILURE);
    }

  memcpy(attrs, default_attrs, sizeof(default_attrs));
  pd->attrs = attrs;
  assert(snprintf(logname, sizeof(logname), "%d", pid) >= 0);

  int fd =
    open(logname, O_WRONLY | O_APPEND | O_CREAT,
         S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

  assert(fd != -1);
  pd->log = fdopen(fd, "a");

  assert(pd->log != NULL);

  for (i = 0; i < nr_counters; i++)
    {
      attrs[i].size = sizeof(struct perf_event_attr);
      attrs[i].inherit = 1;          /* default */
      attrs[i].disabled = 1;         /* disable them now... */
      attrs[i].enable_on_exec = 0;
      pd->fds[i] = sys_perf_event_open(&attrs[i], pid, cpu, -1, 0);
      if (pd->fds[i] < 0)
      {
        fprintf(stderr, "At event %d/%d\n", i, nr_counters);
        perror("sys_perf_event_open");
        // dont exit if it dont exists, just print
        //exit(EXIT_FAILURE);
      }
    }

  pd->wall_start = rdclock();
  return pd;
}

/* thread safe */
/* pass in int* from initialize function */
/* reads from fd's, prints out stats, and closes them all */
void
libperf_finalize(struct libperf_data *pd, void *id)
{
  int i, result, nr_counters = __LIBPERF_ARRAY_SIZE(default_attrs);

  int *fds = pd->fds;

  uint64_t count[3];                 /* potentially 3 values */

  struct stats event_stats[nr_counters];

  struct stats walltime_nsecs_stats;

  for (i = 0; i < nr_counters; i++)
  {
    // in case not all events are available on the system
    if (pd->fds[i] == -1)
      continue;

    assert(fds[i] >= 0);
    result = read(fds[i], count, sizeof(uint64_t));
    assert(result == sizeof(uint64_t));

    update_stats(&event_stats[i], count[0]);

    close(fds[i]);
    fds[i] = -1;

    fprintf(pd->log, "Stats[%p, %d]: %14.0f\n", id, i,
            avg_stats(&event_stats[i]));
  }

  update_stats(&walltime_nsecs_stats, rdclock() - pd->wall_start);
  fprintf(pd->log, "Stats[%p, %d]: %14.9f\n", id, i,
          avg_stats(&walltime_nsecs_stats) / 1e9);
  fclose(pd->log);
  free(pd->attrs);
  free(pd);
}

uint64_t
libperf_readcounter(struct libperf_data *pd, int counter)
{
  uint64_t value;

  assert(counter >= 0 && counter < __LIBPERF_MAX_COUNTERS);

  if (counter == __LIBPERF_MAX_COUNTERS)
    return (uint64_t) (rdclock() - pd->wall_start);

  assert(read(pd->fds[counter], &value, sizeof(uint64_t)) ==
         sizeof(uint64_t));
  
  return value;
}

int
libperf_enablecounter(struct libperf_data *pd, int counter)
{
  assert(counter >= 0 && counter < __LIBPERF_MAX_COUNTERS);
  if (pd->fds[counter] == -1)
    assert((pd->fds[counter] = sys_perf_event_open(&(pd->attrs[counter]), pd->pid, pd->cpu, pd->group, 0)) != -1);

  return ioctl(pd->fds[counter], PERF_EVENT_IOC_ENABLE);
}

int
libperf_disablecounter(struct libperf_data *pd, int counter)
{
  assert(counter >= 0 && counter < __LIBPERF_MAX_COUNTERS);
  if (pd->fds[counter] == -1)
    return 0;
  
  return ioctl(pd->fds[counter], PERF_EVENT_IOC_DISABLE);
}

void
libperf_close(struct libperf_data *pd)
{
  int i, nr_counters = __LIBPERF_ARRAY_SIZE(default_attrs);

  for (i = 0; i < nr_counters; i++)
  {
    // in case not all events are available on the system
    if (pd->fds[i] == -1)
      continue;

    assert(pd->fds[i] >= 0);
    close(pd->fds[i]);
  }
  
  fclose(pd->log);
  free(pd->attrs);
  free(pd);
}

FILE *
libperf_getlogger(struct libperf_data *pd)
{
  return pd->log;
}

int
libperf_unit_test(void *n)
{
  struct libperf_data *pd = libperf_initialize(0, -1);

  char *x = malloc(1024 * 1024 * 1024L);

  unsigned long int i;

  for (i = 0; i < 1024 * 1024 * 1024L; i++)
    x[i] = (char) i;

  fprintf(pd->log, "libperf_readcounter[0]: %" PRIu64 "\n",
          libperf_readcounter(pd, 0));
  
  libperf_finalize(pd, 0);
  free(x);
  return 0;
}
