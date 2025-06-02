#include <time.h>

void delay(uint64_t ms) {
    for (volatile uint32_t i = 0; i < ms * 10000; i++) {
        // Do nothing, just burn time
        __asm__ volatile ("nop");
    }
}

static inline uint64_t rdtsc(void) {
     uint32_t lo;
     uint64_t hi;
     __asm__ volatile("rdtsc" : "=a"(lo), "=d"(hi));
     return (hi << 32) | lo; // Shift 'hi' only 32 bits
}