lib_LTLIBRARIES = libperf.la
check_PROGRAMS = test example benchmark

EXTRA_DIST = libperf.h perf_event.h libperf_example.c libperf_test.c libperf_benchmark.c

libperf_la_SOURCES = libperf.c

libperf_la_LDFLAGS = -version-info $(LIBPERF_SO_VERSION)

include_HEADERS = libperf.h

pkgconfigdir = $(libdir)/pkgconfig

pkgconfig_DATA = libperf.pc

test_SOURCES = libperf_test.c
test_LDADD = libperf.la

example_SOURCES = libperf_example.c
example_LDADD = libperf.la

benchmark_SOURCES = libperf_benchmark.c
benchmark_LDADD = libperf.la
