#include <stdio.h>
#include <stdint.h>

static uint32_t seed = 12345; // Default seed

// Simple Linear Congruential Generator (LCG)
static uint32_t rand_next(void) {
    // Constants for LCG
    uint32_t a = 1664525;
    uint32_t c = 1013904223;
    uint32_t m = 0xFFFFFFFF;
    
    seed = (a * seed + c) & m; // Update seed with LCG formula
    return seed;
}

int rand_main(int argc, char **argv) {
    // If there's no argument, generate a random number
    if (argc == 1) {
        printf("%u\n", rand_next()); // Print random number
    } else {
        // If there's an argument, use it as the upper bound
        uint32_t upper_bound = 0;
        if (argc > 1) {
            // Convert the first argument to an integer (assumed to be a number)
            for (int i = 0; argv[1][i] != '\0'; i++) {
                upper_bound = upper_bound * 10 + (argv[1][i] - '0');
            }
        }
        
        // Ensure the upper bound is greater than 0
        if (upper_bound > 0) {
            uint32_t random_number = rand_next() % (upper_bound + 1); // Generate random number between 0 and upper_bound
            printf("%u\n", random_number); // Print the random number
        } else {
            printf("Invalid upper bound.\n");
        }
    }
    
    return 0;
}
