################################################################################
# Makefile                                                                     #
#                                                                              #
# This is the libperf Makefile.                                                #
#                                                                              #
# libperf interfaces with the kernel performance counters subsystem            #
# Copyright (C) 2010 Wolfgang Richter, Ekaterina Taralova, Karl Naden          #
#                                                                              #
# This program is free software; you can redistribute it and/or                #
# modify it under the terms of the GNU General Public License                  #
# as published by the Free Software Foundation; either version 2               #
# of the License, or (at your option) any later version.                       #
#                                                                              #
# This program is distributed in the hope that it will be useful,              #
# but WITHOUT ANY WARRANTY; without even the implied warranty of               #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                #
# GNU General Public License for more details.                                 #
#                                                                              #
# You should have received a copy of the GNU General Public License            #
# along with this program; if not, write to the Free Software                  #
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                #
# 02110-1301, USA.                                                             #
################################################################################

KERNEL_PERF=-I/home/wolf/Desktop/linux-2.6.32/tools/perf
LIBPERF_DIR=/home/wolf/Dropbox/Projects/LightSpeed/src/libperf

default:
	gcc -Wall $(KERNEL_PERF) -fPIC -c libperf.c
	gcc -Wall -shared -o libperf.so libperf.o -lm -lrt
	gcc -Wall -L`pwd` -lperf libperf_test.c -o libperf_test
	gcc -Wall -L`pwd` -lperf benchmark.c -o bench
	gcc -Wall -L`pwd` -lperf libperf_example.c -o libperf_example

test: default
	LD_LIBRARY_PATH=$(LIBPERF_DIR) ./a.out

clean:
	@rm -f	libperf.o \
		libperf.so \
		libperf_test \
		bench \
		0 \
		libperf_example
