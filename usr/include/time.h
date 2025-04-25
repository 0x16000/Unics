#ifndef TIME_H
#define TIME_H

#include <stdint.h>

// Delays execution for given number of milliseconds
void delay(uint64_t ms);

// Reads the CPU's time stamp counter for super precise timing
static inline uint64_t rdtsc(void);

#endif
