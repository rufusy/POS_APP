#ifdef __PERFORMANCE_LOG__

#ifdef __TELIUM3__
#include "time.h"
#else
#include "OSL_TimeStp.h"
#endif

#ifndef __PERFORMANCE_LOG_MAX_MEASURES__
#define __PERFORMANCE_LOG_MAX_MEASURES__		(10000)
#endif

#ifdef __TELIUM3__
typedef struct perflog_t
{
	const char *string;
	clockid_t clock_id;
	struct timespec time;
} perflog_t;
#else
typedef struct perflog_t
{
	const char *string;
	unsigned long long time;
} perflog_t;
#endif

extern perflog_t perflog_data[__PERFORMANCE_LOG_MAX_MEASURES__];
extern unsigned int perflog_data_size;

#ifdef __TELIUM3__

#define perflog_withid(clk_id,str)	{ if (perflog_data_size < __PERFORMANCE_LOG_MAX_MEASURES__) \
		{ clock_gettime((clk_id), &perflog_data[perflog_data_size].time); \
		perflog_data[perflog_data_size].string = (str); \
		perflog_data[perflog_data_size].clock_id = (clk_id); \
		perflog_data_size++; } }

#define perflog_process(str)	perflog_withid(CLOCK_PROCESS_CPUTIME_ID, (str))
#define perflog(str)			perflog_withid(CLOCK_MONOTONIC_RAW, (str));

void perflog_dump(void);

#else

#define perflog(str) { if (perflog_data_size < __PERFORMANCE_LOG_MAX_MEASURES__) \
		{ perflog_data[perflog_data_size].time = OSL_TimeStp_Now(); \
		perflog_data[perflog_data_size].string = str; \
		perflog_data_size++; } }

#define perflog_withid(clk_id, str) { if ((clk_id != CLOCK_PROCESS_CPUTIME_ID) && (clk_id != CLOCK_THREAD_CPUTIME_ID)) \
		{ perflog(str) }
#define perflog_process(str)				((void)0)

void perflog_dump(void);

#endif

#else

#define perflog_withid(clk_id, str)			((void)0)
#define perflog_process(str)				((void)0)
#define perflog(str)						((void)0)
#define perflog_dump()						((void)0)

#endif
