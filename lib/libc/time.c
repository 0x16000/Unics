#include <time.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Simple delay using busy-wait
void delay(uint64_t ms) {
    for (volatile uint32_t i = 0; i < ms * 10000; i++) {
        __asm__ volatile ("nop");
    }
}

// Get current time (simplified implementation)
time_t time(time_t *tloc) {
    // This would normally read from a system clock
    static time_t current_time = 0;
    current_time++; // Simulate time passing
    
    if (tloc != NULL) {
        *tloc = current_time;
    }
    return current_time;
}

// Convert time_t to UTC time
struct tm *gmtime(const time_t *timer) {
    static struct tm result;
    time_t t = timer ? *timer : time(NULL);
    
    // Simplified conversion
    result.tm_sec = t % 60;
    result.tm_min = (t / 60) % 60;
    result.tm_hour = (t / 3600) % 24;
    result.tm_mday = (t / 86400) % 31 + 1;
    result.tm_mon = (t / 2678400) % 12;
    result.tm_year = (t / 32140800) + 70;
    result.tm_wday = (t / 86400) % 7;
    result.tm_yday = (t / 86400) % 365;
    result.tm_isdst = -1;
    result.tm_nsec = 0;
    
    return &result;
}

// Convert time_t to local time
struct tm *localtime(const time_t *timer) {
    return gmtime(timer); // No timezone support in this simple version
}

// Convert tm structure to time_t
time_t mktime(struct tm *tm) {
    return tm->tm_sec + tm->tm_min * 60 + tm->tm_hour * 3600 +
           tm->tm_mday * 86400 + tm->tm_mon * 2678400 +
           (tm->tm_year - 70) * 32140800;
}

// Format time as string
char *asctime(const struct tm *tm) {
    static char result[26];
    const char *wday[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    const char *mon[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                         "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    
    snprintf(result, sizeof(result), "%.3s %.3s%3d %.2d:%.2d:%.2d %d\n",
             wday[tm->tm_wday], mon[tm->tm_mon], tm->tm_mday,
             tm->tm_hour, tm->tm_min, tm->tm_sec, 1900 + tm->tm_year);
    return result;
}

// Convert time_t to string
char *ctime(const time_t *timer) {
    return asctime(gmtime(timer));
}

// Format time according to format string
size_t strftime(char *s, size_t max, const char *format, const struct tm *tm) {
    (void)format; // Unused parameter
    return snprintf(s, max, "%04d-%02d-%02d %02d:%02d:%02d",
                   1900 + tm->tm_year, tm->tm_mon + 1, tm->tm_mday,
                   tm->tm_hour, tm->tm_min, tm->tm_sec);
}

// High precision sleep
int nanosleep(const struct timespec *req, struct timespec *rem) {
    // Avoid 64-bit arithmetic in 32-bit mode
    uint32_t ms = (uint32_t)(req->tv_sec * 1000) + (uint32_t)(req->tv_nsec / 1000000);
    delay(ms);
    if (rem) {
        rem->tv_sec = 0;
        rem->tv_nsec = 0;
    }
    return 0;
}

// Time difference calculation
double difftime(time_t time1, time_t time0) {
    return (double)(time1 - time0);
}

// Get time from specific clock
int clock_gettime(clockid_t clk_id, struct timespec *tp) {
    (void)clk_id; // Unused parameter
    
    if (!tp) return -1;
    
    // Avoid 64-bit division/modulo operations
    static uint32_t counter = 0;
    tp->tv_sec = counter / 10;  // 100ms units
    tp->tv_nsec = (counter % 10) * 100000000;
    counter++;
    
    return 0;
}

// Set time (not implemented)
int clock_settime(clockid_t clk_id, const struct timespec *tp) {
    (void)clk_id; // Unused parameter
    (void)tp;     // Unused parameter
    return -1; // Not supported
}

// Get clock resolution
int clock_getres(clockid_t clk_id, struct timespec *res) {
    (void)clk_id; // Unused parameter
    
    if (!res) return -1;
    res->tv_sec = 0;
    res->tv_nsec = 1000000; // 1ms resolution
    return 0;
}