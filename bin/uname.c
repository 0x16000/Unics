#include <string.h>
#include <stdio.h>
#include <vers.h>

// Function to print usage instructions
static void print_usage() {
    printf("Usage: uname [OPTION]...\n");
    printf("Print system information.\n\n");
    printf("Options:\n");
    printf("  -a     Print all information\n");
    printf("  -s     Print kernel name\n");
    printf("  -n     Print network node hostname\n");
    printf("  -r     Print kernel release\n");
    printf("  -m     Print machine architecture\n");
    printf("  -v     Print kernel build version\n");
    printf("  -o     Print operating system\n");
    printf("  --help Display this help\n");
}

// Main uname function
int uname_main(int argc, char **argv) {
    // Default to print OS name if no arguments are provided
    if (argc == 1) {
        printf("%s\n", OS_NAME);
        return 0;
    }

    // Parse the arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            print_usage();
            return 0;
        } else if (strcmp(argv[i], "-a") == 0) {
            // Print all information
            printf("%s %s %s %s %s %s\n", OS_NAME, OS_HOSTNAME, OS_VERSION,
                   OS_BUILD_VERSION, OS_ARCH, OS);
        } else if (strcmp(argv[i], "-s") == 0) {
            printf("%s\n", OS_NAME);  // Kernel name
        } else if (strcmp(argv[i], "-n") == 0) {
            printf("%s\n", OS_HOSTNAME);  // Hostname
        } else if (strcmp(argv[i], "-r") == 0) {
            printf("%s\n", OS_VERSION);  // Kernel version
        } else if (strcmp(argv[i], "-m") == 0) {
            printf("%s\n", OS_ARCH);  // Machine architecture
        } else if (strcmp(argv[i], "-v") == 0) {
            printf("%s\n", OS_BUILD_VERSION);  // Build version
        } else if (strcmp(argv[i], "-o") == 0) {
            printf("%s\n", OS);  // Operating system
        } else {
            printf("uname: invalid option '%c'\n", argv[i][1]);
            print_usage();
            return 1;  // Error code for invalid option
        }
    }

    return 0;
}
