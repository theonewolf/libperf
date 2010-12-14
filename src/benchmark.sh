#!/bin/bash

###############################################################################
# benchmark.sh                                                                #
#                                                                             #
# This bash script runs a series of benchmarks using the benchmark tool       #
# (benchmark.c).  It saves the output in a series of log files.               #
#                                                                             #
# libperf interfaces with the kernel performance counters subsystem           #
# Copyright (C) 2010 Wolfgang Richter, Ekaterina Taralova, Karl Naden         #
#                                                                             #
# This program is free software; you can redistribute it and/or               #
# modify it under the terms of the GNU General Public License                 #
# as published by the Free Software Foundation; either version 2              #
# of the License, or (at your option) any later version.                      #
#                                                                             #
# This program is distributed in the hope that it will be useful,             #
# but WITHOUT ANY WARRANTY; without even the implied warranty of              #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               #
# GNU General Public License for more details.                                #
#                                                                             #
# You should have received a copy of the GNU General Public License           #
# along with this program; if not, write to the Free Software                 #
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA               #
# 02110-1301, USA.                                                            #
###############################################################################

TRIALS="{1..10}"

# perf wrapped
for i in $TRIALS; do sudo ./bench 0 >> cpu.libperf.log; done
for i in $TRIALS; do sudo ./bench 3 >> mem.libperf.log; done
for i in $TRIALS; do sudo ./bench 1  >> threads.libperf.log; done
for i in $TRIALS; do sudo ./bench 5  >> IO.libperf.log; done
for i in $TRIALS; do sudo ./bench 2  >> mutex.libperf.log; done

# bare
for i in $TRIALS; do sudo /usr/bin/sysbench --test=cpu --cpu-max-prime=20000 run >> cpu.bare.log; done
for i in $TRIALS; do sudo /usr/bin/sysbench --test=memory --memory-block-size=4K --memory-total-size=23G run  >> mem.bare.log; done
for i in $TRIALS; do sudo /usr/bin/sysbench --num-threads=64 --test=threads --thread-yields=7500 --thread-locks=8 run  >> threads.bare.log; done
for i in $TRIALS; do sudo /usr/bin/sysbench --num-threads=2 --max-requests=7000 --test=fileio --file-total-size=3G --file-test-mode=rndrw run  >> IO.bare.log; done
for i in $TRIALS; do sudo /usr/bin/sysbench --test=mutex --mutex-num=4096 --mutex-locks=300000000 --mutex-loops=10000000 run  >> mutex.bare.log; done
