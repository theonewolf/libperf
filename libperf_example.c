#include <inttypes.h> /* for PRIu64 definition */
#include <stdint.h>   /* for uint64_t and PRIu64 */
#include <stdio.h>    /* for printf family */
#include <stdlib.h>   /* for EXIT_SUCCESS definition */
#include "libperf.h"  /* standard libperf include */

int main(int argc, char* argv[])
{
	struct perf_data* pd = libperf_initialize(-1,-1); /* init lib */
	uint64_t counter = libperf_readcounter(pd, LIBPERF_COUNT_HW_INSTRUCTIONS); /* obtain counter value */

	fprintf(stdout, "counter read: %"PRIu64"\n", counter); /* printout */
	
	FILE* log = libperf_getlogger(pd); /* get log file stream */
	fprintf(log, "custom log message\n"); /* print a custom log message */

	libperf_finalize(pd, 0); /* log all counter values */

	return EXIT_SUCCESS; /* success exit value */
}
