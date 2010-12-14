/******************************************************************************
 * perf-event.h                                                               *
 *                                                                            *
 * This file defines the interface into the perf_event_open syscall.          *
 * Taken from the Linux Kernel source tree.                                   *
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

/*
 * Performance events:
 *
 *    Copyright (C) 2008-2009, Thomas Gleixner <tglx@linutronix.de>
 *    Copyright (C) 2008-2009, Red Hat, Inc., Ingo Molnar
 *    Copyright (C) 2008-2009, Red Hat, Inc., Peter Zijlstra
 *
 * Data type definitions, declarations, prototypes.
 *
 *    Started by: Thomas Gleixner and Ingo Molnar
 *
 * For licencing details see kernel-base/COPYING
 */

#ifndef _LIBPERF_PERF_EVENT_H
#define _LIBPERF_PERF_EVENT_H

#include <linux/types.h>
#include <linux/ioctl.h>
#include <asm/byteorder.h>

/*
 * User-space ABI bits:
 */

/*
 * attr.type
 */
enum perf_type_id {
  PERF_TYPE_HARDWARE      = 0,
  PERF_TYPE_SOFTWARE      = 1,
  PERF_TYPE_TRACEPOINT    = 2,
  PERF_TYPE_HW_CACHE      = 3,
  PERF_TYPE_RAW           = 4,
  PERF_TYPE_BREAKPOINT    = 5,

  PERF_TYPE_MAX,        /* non-ABI */
};

/*
 * Generalized performance event event_id types, used by the
 * attr.event_id parameter of the sys_perf_event_open()
 * syscall:
 */
enum perf_hw_id {
  /*
   * Common hardware events, generalized by the kernel:
   */
  PERF_COUNT_HW_CPU_CYCLES          = 0,
  PERF_COUNT_HW_INSTRUCTIONS        = 1,
  PERF_COUNT_HW_CACHE_REFERENCES    = 2,
  PERF_COUNT_HW_CACHE_MISSES        = 3,
  PERF_COUNT_HW_BRANCH_INSTRUCTIONS = 4,
  PERF_COUNT_HW_BRANCH_MISSES       = 5,
  PERF_COUNT_HW_BUS_CYCLES          = 6,

  PERF_COUNT_HW_MAX,      /* non-ABI */
};

/*
 * Generalized hardware cache events:
 *
 *       { L1-D, L1-I, LLC, ITLB, DTLB, BPU } x
 *       { read, write, prefetch } x
 *       { accesses, misses }
 */
enum perf_hw_cache_id {
  PERF_COUNT_HW_CACHE_L1D     = 0,
  PERF_COUNT_HW_CACHE_L1I     = 1,
  PERF_COUNT_HW_CACHE_LL      = 2,
  PERF_COUNT_HW_CACHE_DTLB    = 3,
  PERF_COUNT_HW_CACHE_ITLB    = 4,
  PERF_COUNT_HW_CACHE_BPU     = 5,

  PERF_COUNT_HW_CACHE_MAX,    /* non-ABI */
};

enum perf_hw_cache_op_id {
  PERF_COUNT_HW_CACHE_OP_READ     = 0,
  PERF_COUNT_HW_CACHE_OP_WRITE    = 1,
  PERF_COUNT_HW_CACHE_OP_PREFETCH = 2,

  PERF_COUNT_HW_CACHE_OP_MAX,   /* non-ABI */
};

enum perf_hw_cache_op_result_id {
  PERF_COUNT_HW_CACHE_RESULT_ACCESS = 0,
  PERF_COUNT_HW_CACHE_RESULT_MISS   = 1,

  PERF_COUNT_HW_CACHE_RESULT_MAX,   /* non-ABI */
};

/*
 * Special "software" events provided by the kernel, even if the hardware
 * does not support performance events. These events measure various
 * physical and sw events of the kernel (and allow the profiling of them as
 * well):
 */
enum perf_sw_ids {
  PERF_COUNT_SW_CPU_CLOCK           = 0,
  PERF_COUNT_SW_TASK_CLOCK          = 1,
  PERF_COUNT_SW_PAGE_FAULTS         = 2,
  PERF_COUNT_SW_CONTEXT_SWITCHES    = 3,
  PERF_COUNT_SW_CPU_MIGRATIONS      = 4,
  PERF_COUNT_SW_PAGE_FAULTS_MIN     = 5,
  PERF_COUNT_SW_PAGE_FAULTS_MAJ     = 6,
  PERF_COUNT_SW_ALIGNMENT_FAULTS    = 7,
  PERF_COUNT_SW_EMULATION_FAULTS    = 8,

  PERF_COUNT_SW_MAX,      /* non-ABI */
};

/*
 * Hardware event_id to monitor via a performance monitoring event:
 */
struct perf_event_attr {

  /*
   * Major type: hardware/software/tracepoint/etc.
   */
  __u32     type;

  /*
   * Size of the attr structure, for fwd/bwd compat.
   */
  __u32     size;

  /*
   * Type specific configuration information.
   */
  __u64     config;

  union {
    __u64   sample_period;
    __u64   sample_freq;
  };

  __u64     sample_type;
  __u64     read_format;

  __u64 disabled       :  1, /* off by default        */
        inherit        :  1, /* children inherit it   */
        pinned         :  1, /* must always be on PMU */
        exclusive      :  1, /* only group on PMU     */
        exclude_user   :  1, /* don't count user      */
        exclude_kernel :  1, /* ditto kernel          */
        exclude_hv     :  1, /* ditto hypervisor      */
        exclude_idle   :  1, /* don't count when idle */
        mmap           :  1, /* include mmap data     */
        comm           :  1, /* include comm data     */
        freq           :  1, /* use freq, not period  */
        inherit_stat   :  1, /* per task counts       */
        enable_on_exec :  1, /* next exec enables     */
        task           :  1, /* trace fork/exit       */
        watermark      :  1, /* wakeup_watermark      */
        /*
         * precise_ip:
         *
         *  0 - SAMPLE_IP can have arbitrary skid
         *  1 - SAMPLE_IP must have constant skid
         *  2 - SAMPLE_IP requested to have 0 skid
         *  3 - SAMPLE_IP must have 0 skid
         *
         *  See also PERF_RECORD_MISC_EXACT_IP
         */
        precise_ip     :  2, /* skid constraint       */

        __reserved_1   : 47;

  union {
    __u32   wakeup_events;    /* wakeup every n events */
    __u32   wakeup_watermark; /* bytes before wakeup   */
  };

  __u32     bp_type;
  __u64     bp_addr;
  __u64     bp_len;
};

/*
 * Ioctls that can be done on a perf event fd:
 */
#define PERF_EVENT_IOC_ENABLE   _IO ('$', 0)
#define PERF_EVENT_IOC_DISABLE    _IO ('$', 1)

#endif /* __LIBPERF_PERF_EVENT_H */
