#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([libperf], [0.1], [libperf@googlegroups.com], [libperf], [https://github.com])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([build-aux/m4])
AM_INIT_AUTOMAKE([1.11 foreign -Wall -Werror])
AM_SILENT_RULES([yes])
AC_CONFIG_SRCDIR([src/libperf.h])
AC_CONFIG_HEADERS([src/common/config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AM_PROG_AR

# Checks for libraries.
AC_CHECK_LIB([m], [sin])
AC_CHECK_LIB([rt], [clock_gettime])

# Initialize libtool
LT_INIT

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h inttypes.h stdint.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_UINT64_T

# libperf substitutions
AC_SUBST([LIBPERF_SO_VERSION], [0:0:0])
AC_SUBST([LIBPERF_API_VERSION],[1.0])

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_CHECK_FUNCS([clock_gettime])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/libperf.pc:src/libperf.pc.in])
AC_OUTPUT
