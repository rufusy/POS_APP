#ifdef __PERFORMANCE_LOG__

#include "perf_log.h"

#ifdef __TELIUM3__
#include "Sys_log.h"
#else
#include "sdk.h"
#endif

perflog_t perflog_data[__PERFORMANCE_LOG_MAX_MEASURES__];
unsigned int perflog_data_size = 0;

#ifdef __TELIUM3__

void perflog_dump(void) {
	unsigned int i;
	for(i = 0; i < perflog_data_size; i++)
		Sys_log(4, "%s\t%i\t%llu", perflog_data[i].string, perflog_data[i].clock_id, (((unsigned long long)perflog_data[i].time.tv_sec) * 1000000000ULL) + perflog_data[i].time.tv_nsec);
	perflog_data_size = 0;
}

#else

void perflog_dump(void) {
	unsigned int i;
	char string[256];
	int lg;
	unsigned long a;
	unsigned long b;
	unsigned long c;

	for(i = 0; i < perflog_data_size; i++) {
		if (perflog_data[i].time < 4294967296ULL)
		{
			lg = sprintf(string, "%s\t%i\t%lu,", perflog_data[i].string, 0, (unsigned long)perflog_data[i].time);
		}
		else
		{
			a = (unsigned long)(perflog_data[i].time % 1000000000ULL);
			b = (unsigned long)((perflog_data[i].time / 1000000000ULL) % 1000000000ULL);
			c = (unsigned long)((perflog_data[i].time / 1000000000000000000ULL) % 1000000000ULL);

			if (c > 0)
			{
				lg = sprintf(string, "%s\t%i\t%lu%09lu%09lu", perflog_data[i].string, 0, c, b, a);
			}
			else if (b > 0)
			{
				lg = sprintf(string, "%s\t%i\t%lu%09lu", perflog_data[i].string, 0, b, a);
			}
			else
			{
				lg = sprintf(string, "%s\t%i\t%lu", perflog_data[i].string, 0, a);
			}
		}
		trace(0, lg, string);
	}
	perflog_data_size = 0;
}

#endif

#endif
