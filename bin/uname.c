#include <string.h>
#include <stdio.h>
#include <vers.h>

// Function to print usage instructions
static void print_usage() {
    printf("Usage: uname [OPTION]...\n");
    printf("Print system information.\n\n");
    printf("Options:\n");
    printf(" -a Print all information\n");
    printf(" -s Print kernel name\n");
    printf(" -n Print network node hostname\n");
    printf(" -r Print kernel release\n");
    printf(" -m Print machine architecture\n");
    printf(" -v Print kernel build version\n");
    printf(" -o Print operating system\n");
    printf(" --help Display this help\n");
}

// Main uname function
int uname_main(int argc, char **argv) {
    // Default to print OS name if no arguments are provided
    if (argc == 1) {
        printf("%s\n", OS_NAME);
        return 0;
    }

    int printed_something = 0;
    
    // Parse the arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            print_usage();
            return 0;
        } else if (strcmp(argv[i], "-a") == 0) {
            // Print all information
            printf("%s %s %s %s %s %s", OS_NAME, OS_HOSTNAME, OS_VERSION,
                   OS_BUILD_VERSION, OS_ARCH, OS);
            printed_something = 1;
        } else if (strcmp(argv[i], "-s") == 0) {
            if (printed_something) printf(" ");
            printf("%s", OS_NAME); // Kernel name
            printed_something = 1;
        } else if (strcmp(argv[i], "-n") == 0) {
            if (printed_something) printf(" ");
            printf("%s", OS_HOSTNAME); // Hostname
            printed_something = 1;
        } else if (strcmp(argv[i], "-r") == 0) {
            if (printed_something) printf(" ");
            printf("%s", OS_VERSION); // Kernel version
            printed_something = 1;
        } else if (strcmp(argv[i], "-m") == 0) {
            if (printed_something) printf(" ");
            printf("%s", OS_ARCH); // Machine architecture
            printed_something = 1;
        } else if (strcmp(argv[i], "-v") == 0) {
            if (printed_something) printf(" ");
            printf("%s", OS_BUILD_VERSION); // Build version
            printed_something = 1;
        } else if (strcmp(argv[i], "-o") == 0) {
            if (printed_something) printf(" ");
            printf("%s", OS); // Operating system
            printed_something = 1;
        } else {
            // Handle both single-char and multi-char invalid options
            if (argv[i][0] == '-' && argv[i][1] != '\0' && argv[i][2] == '\0') {
                printf("uname: invalid option '%s'\n", argv[i]);
            } else {
                printf("uname: invalid option '%s'\n", argv[i]);
            }
            print_usage();
            return 1; // Error code for invalid option
        }
    }
    
    if (printed_something) {
        printf("\n");
    }
    
    return 0;
}
