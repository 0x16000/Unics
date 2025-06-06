// usr/include/time.h
#ifndef TIME_H
#define TIME_H

#include <stdint.h>
#include <stddef.h>

// Define basic time types
typedef int64_t time_t;        // Time in seconds
typedef int clockid_t;         // Clock identifier type
typedef uint64_t timer_t;      // Timer identifier type

// Time structure (compatible with standard tm but with nanoseconds)
struct tm {
    int tm_sec;     // seconds [0-60] (1 leap second)
    int tm_min;     // minutes [0-59]
    int tm_hour;    // hours [0-23]
    int tm_mday;    // day of month [1-31]
    int tm_mon;     // month [0-11]
    int tm_year;    // years since 1900
    int tm_wday;    // day of week [0-6] (Sunday = 0)
    int tm_yday;    // day of year [0-365]
    int tm_isdst;   // daylight savings flag
    long tm_nsec;   // nanoseconds [0-999999999]
};

// Timespec structure for high-resolution time
struct timespec {
    time_t tv_sec;  // seconds
    long tv_nsec;   // nanoseconds
};

// Itimerspec structure for interval timers
struct itimerspec {
    struct timespec it_interval;  // Timer period
    struct timespec it_value;     // Timer expiration
};

// Timezone structure
struct timezone {
    int tz_minuteswest; // minutes west of Greenwich
    int tz_dsttime;     // type of DST correction
};

// Clock types for clock_gettime()
#define CLOCK_REALTIME            0
#define CLOCK_MONOTONIC           1
#define CLOCK_PROCESS_CPUTIME_ID  2
#define CLOCK_THREAD_CPUTIME_ID   3
#define CLOCK_MONOTONIC_RAW       4
#define CLOCK_REALTIME_COARSE     5
#define CLOCK_MONOTONIC_COARSE    6
#define CLOCK_BOOTTIME            7
#define CLOCK_REALTIME_ALARM      8
#define CLOCK_BOOTTIME_ALARM      9

// Delays execution for given number of milliseconds
void delay(uint64_t ms);

// High precision sleep (nanoseconds)
int nanosleep(const struct timespec *req, struct timespec *rem);

// Get current time with nanosecond precision
int clock_gettime(clockid_t clk_id, struct timespec *tp);

// Set time
int clock_settime(clockid_t clk_id, const struct timespec *tp);

// Get resolution of specific clock
int clock_getres(clockid_t clk_id, struct timespec *res);

// CPU ticks since startup (for precise timing)
static inline uint64_t rdtsc(void) {
    uint32_t lo, hi;
    __asm__ volatile("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

// Standard time functions
time_t time(time_t *tloc);
struct tm *gmtime(const time_t *timer);
struct tm *localtime(const time_t *timer);
time_t mktime(struct tm *tm);
char *asctime(const struct tm *tm);
char *ctime(const time_t *timer);
size_t strftime(char *s, size_t max, const char *format, const struct tm *tm);

// Timer functions
struct sigevent;  // Forward declaration
int timer_create(clockid_t clockid, struct sigevent *sevp, timer_t *timerid);
int timer_delete(timer_t timerid);
int timer_settime(timer_t timerid, int flags, 
                 const struct itimerspec *new_value,
                 struct itimerspec *old_value);
int timer_gettime(timer_t timerid, struct itimerspec *curr_value);

// Time difference calculation
double difftime(time_t time1, time_t time0);

#endif // TIME_H