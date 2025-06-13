#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>

void print_usage(const char *progname) {
    fprintf(stderr, "Usage: %s <time>\n", progname);
    fprintf(stderr, "Pauses execution for the specified time (100 units = 1 second)\n");
}

int sleep_main(int argc, char **argv) {
    if (argc != 2) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    // Parse the sleep duration
    char *endptr;
    double seconds = strtod(argv[1], &endptr);
    
    // Check for conversion errors
    if (*endptr != '\0' || seconds < 0) {
        fprintf(stderr, "Error: Invalid time specification '%s'\n", argv[1]);
        return EXIT_FAILURE;
    }

    // Split into seconds and nanoseconds
    time_t sec = (time_t)seconds;
    long nsec = (long)((seconds - sec) * 1e9);

    // Set up the timespec structure
    struct timespec req = {
        .tv_sec = sec,
        .tv_nsec = nsec
    };

    // Perform the sleep
    while (nanosleep(&req, &req)) {
        if (errno != EINTR) {
            perror("sleep");
            return EXIT_FAILURE;
        }
        // If interrupted by a signal, continue sleeping with remaining time
    }

    return EXIT_SUCCESS;
}

