#!/bin/bash

###############################################################################
# code_style.sh                                                               #
#                                                                             #
# This bash script formats code to conform to the styles used by libperf.     #
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

indent -blf -bls -bli0 -cli0 -cs -nut -psl -saf -sai -saw -ts4 -npcs -nprs -bbb -bad -bl -nce  -lc80 -st $1 
