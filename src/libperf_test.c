/******************************************************************************
 * libperf_test.c                                                             *
 *                                                                            *
 * This is a small example program demoing linking with and using libperf,    *
 * used for unit testing.                                                     *
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

#include <stdio.h>
#include "libperf.h"

int main(int argc, char* argv[])
{
	void * n = 0;
	int ret = libperf_unit_test(n);
	struct libperf_data* pd = libperf_initialize(0,-1);
	fprintf(stdout, "ret: %d\n", ret);
	libperf_finalize(pd,n);
	return 0;
}
