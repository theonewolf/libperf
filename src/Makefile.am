lib_LTLIBRARIES = libperf.la

EXTRA_DIST = libperf.h

libperf_la_SOURCES = libperf.c

libperf_la_LDFLAGS = -version-info $(LIBPERF_SO_VERSION)

libperf_includedir = $(includedir)/libperf/libperf

libperf_include_HEADERS = libperf.h

pkgconfigdir = $(libdir)/pkgconfig

pkgconfig_DATA = libperf.pc
