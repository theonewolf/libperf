/******************************************************************************
 * libperf.h                                                                  *
 *                                                                            *
 * This file defines the libperf interface.  libperf is a library that wraps  *
 * around the syscall sys_perf_event_open( ).  This library exposes the       *
 * kernel performance counters subsystem to userspace code.  It can be used   *
 * to efficiently trace portions of code with kernel support.  Tracing        *
 * individual tasks/pids requires root access (as to some other functional    *
 * components).                                                               *
 *                                                                            *
 * This system call interface was first announced in 2009 by Ingo Molnar      *
 * (http://lkml.org/lkml/2009/6/6/149), and is now included in the Linux      *
 * kernel mainline.                                                           *
 *                                                                            *
 * Note: libperf is thread safe.  Multiple threads may use this library, but  *
 * each thread should call libperf_initialize separately and use their own    *
 * context returned from that call for subsequent library calls.              *
 *                                                                            *
 * Authors: Wolfgang Richter   <wolf@cs.cmu.edu>,                             *
 *          Ekaterina Taralova <etaralova@cs.cmu.edu>,                        *
 *          Karl Naden         <kbn@cs.cmu.edu>                               *
 *                                                                            *
 *                                                                            *
 * libperf interfaces with the kernel performance counters subsystem          *
 * Copyright (C) 2010  Wolfgang Richter, Ekaterina Taralova, Karl Naden       *
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

#ifndef _LIBPERF_H
#define _LIBPERF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

/* lib struct */
struct perf_data;

/* lib constants */
enum libperf_tracepoint
{
	/* sw tracepoints */
	LIBPERF_COUNT_SW_CPU_CLOCK = 0,
	LIBPERF_COUNT_SW_TASK_CLOCK = 1,
	LIBPERF_COUNT_SW_CONTEXT_SWITCHES = 2,
	LIBPERF_COUNT_SW_CPU_MIGRATIONS = 3,
	LIBPERF_COUNT_SW_PAGE_FAULTS = 4,
	LIBPERF_COUNT_SW_PAGE_FAULTS_MIN = 5,
	LIBPERF_COUNT_SW_PAGE_FAULTS_MAJ = 6,

	/* hw counters */
	LIBPERF_COUNT_HW_CPU_CYCLES = 7,
	LIBPERF_COUNT_HW_INSTRUCTIONS = 8,
	LIBPERF_COUNT_HW_CACHE_REFERENCES = 9,
	LIBPERF_COUNT_HW_CACHE_MISSES = 10,
	LIBPERF_COUNT_HW_BRANCH_INSTRUCTIONS = 11,
	LIBPERF_COUNT_HW_BRANCH_MISSES = 12,
	LIBPERF_COUNT_HW_BUS_CYCLES = 13,

	/* cache counters */

	/* L1D - data cache */
	LIBPERF_COUNT_HW_CACHE_L1D_LOADS = 14,
	LIBPERF_COUNT_HW_CACHE_L1D_LOADS_MISSES = 15,
	LIBPERF_COUNT_HW_CACHE_L1D_STORES = 16,
	LIBPERF_COUNT_HW_CACHE_L1D_STORES_MISSES = 17,
	LIBPERF_COUNT_HW_CACHE_L1D_PREFETCHES = 18,

	/* L1I - instruction cache */
	LIBPERF_COUNT_HW_CACHE_L1I_LOADS = 19,
	LIBPERF_COUNT_HW_CACHE_L1I_LOADS_MISSES = 20,

	/* LL - last level cache */
	LIBPERF_COUNT_HW_CACHE_LL_LOADS = 21,
	LIBPERF_COUNT_HW_CACHE_LL_LOADS_MISSES = 22,
	LIBPERF_COUNT_HW_CACHE_LL_STORES  = 23,
	LIBPERF_COUNT_HW_CACHE_LL_STORES_MISSES = 24,
	
	/* DTLB - data translation lookaside buffer */
	LIBPERF_COUNT_HW_CACHE_DTLB_LOADS = 25,
	LIBPERF_COUNT_HW_CACHE_DTLB_LOADS_MISSES = 26,
	LIBPERF_COUNT_HW_CACHE_DTLB_STORES = 27,
	LIBPERF_COUNT_HW_CACHE_DTLB_STORES_MISSES = 28,

	/* ITLB - instructiont translation lookaside buffer */
	LIBPERF_COUNT_HW_CACHE_ITLB_LOADS = 29,
	LIBPERF_COUNT_HW_CACHE_ITLB_LOADS_MISSES = 30,

	/* BPU - branch prediction unit */
	LIBPERF_COUNT_HW_CACHE_BPU_LOADS = 31,
	LIBPERF_COUNT_HW_CACHE_BPU_LOADS_MISSES = 32,

	/* Special internally defined "counter" */
	/* this is the _only_ floating point value */
	LIBPERF_LIB_SW_WALL_TIME = 33
};

/* libperf_initialize
 *
 * This function initializes the library.
 *
 * int pid - pass in gettid()/getpid() value, -1 for current process
 * int cpu - pass in cpuid to track, -1 for any
 *
 * return - perf_data structure for use in future library calls
 */
struct perf_data *
libperf_initialize(int pid, int cpu);

/* libperf_finalize
 *
 * This function performs cleanup and logs all counters for
 * debugging/logging purposes. Log files are named after the
 * pid/tid that was used when calling libperf_initialize, and
 * they are saved in the same location where the binary is run from.
 *
 * struct perf_data* pd - library structure obtained from libperf_initialize()
 * void* id - a unique identifier to tag log messages
 */
void
libperf_finalize(struct perf_data *pd, void *id);

/* libperf_readcounter
 *
 * This function reads an individual counter.
 *
 * struct perf_data* pd - library structure obtained from libperf_initialize()
 * int counter - pass in a constant value defined in enum libperf_tracepoint
 *
 * Proper method of printing value:
 *
 *
 *      #define __STDC_FORMAT_MACROS
 *      #include <inttypes.h>
 *      #include <stdio.h>
 *
 *      ...
 *
 *      fprintf(stdout, "%" PRIu64 "\n", value);
 *
 *
 * return - 64 bit counter value
 */
uint64_t
libperf_readcounter(struct perf_data *pd, int counter);

/* libperf_enablecounter
 *
 * This function enables an individual counter.
 *
 * struct perf_data* pd - library structure obtained from libperf_initialize()
 * int counter - pass in a constant value defined in enum libperf_tracepoint
 *
 * return - same semantics as ioctl 
 */
int
libperf_enablecounter(struct perf_data *pd, int counter);

/* libperf_disablecounter
 *
 * This function disables an individual counter.
 *
 * struct perf_data* pd - library structure obtained from libperf_initialize()
 * int counter - pass in a constant value defined in enum libperf_tracepoint
 *
 * return - same semantics as ioctl 
 */
int
libperf_disablecounter(struct perf_data *pd, int counter);

/* libperf_close
 *
 * This function shuts down the library performing cleanup.
 * It should always be called when you're finished tracing to avoid memory
 * leaks.
 *
 * struct perf_data* pd - library structure obtained from libperf_initialize()
 */
void
libperf_close(struct perf_data *pd);

/* libperf_getlogger
 *
 * This function returns a file descriptor to the libperf log file.
 *
 * return - file descriptor to the log file associated with pd
 */
FILE *
libperf_getlogger(struct perf_data *pd);

/* libperf_unit_test
 *
 * This function performs some small unit testing of the library.
 *
 * void* - can be any random value, doesn't matter (just testing)
 *
 * return - always should return 0, there is no failure value
 */
int
libperf_unit_test(void *);

#ifdef __cplusplus
}
#endif
#endif /* __LIBPERF_H */
