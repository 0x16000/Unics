#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

static uint32_t seed = 12345; // Default seed

// Simple Linear Congruential Generator (LCG)
static uint32_t rand_next(void) {
    const uint32_t a = 1664525;
    const uint32_t c = 1013904223;
    seed = (a * seed + c) & 0xFFFFFFFF; // Update seed with LCG formula
    return seed;
}

// Helper function to parse a positive integer from a string
static bool parse_uint32(const char *str, uint32_t *result) {
    *result = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] < '0' || str[i] > '9') {
            return false; // Non-digit character
        }
        if (*result > (UINT32_MAX / 10)) {
            return false; // Overflow
        }
        *result = *result * 10 + (str[i] - '0');
    }
    return true;
}

int rand_main(int argc, char **argv) {
    if (argc == 1) {
        printf("%u\n", rand_next()); // Print random number
        return 0;
    }

    uint32_t upper_bound;
    if (!parse_uint32(argv[1], &upper_bound)) {
        printf("Error: Upper bound must be a positive integer.\n");
        return 1;
    }

    if (upper_bound == 0) {
        printf("Error: Upper bound must be at least 1.\n");
        return 1;
    }

    uint32_t range = upper_bound + 1;
    uint32_t max_allowed = 0xFFFFFFFF / range * range; // Avoid bias
    uint32_t random_number;
    do {
        random_number = rand_next();
    } while (random_number >= max_allowed);

    printf("%u\n", random_number % range);
    return 0;
}
