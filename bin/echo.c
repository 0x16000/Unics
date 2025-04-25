#include <stdio.h>

int echo_main(int argc, char **argv) {
    // If no arguments, print a newline
    if (argc == 1) {
        printf("\n");
        return 0;
    }

    // Loop through all arguments and print them
    for (int i = 1; i < argc; i++) {
        printf("%s", argv[i]);  // Print the argument
        if (i < argc - 1) {
            printf(" ");  // Add space between arguments
        }
    }
    printf("\n");  // Print newline after all arguments
    return 0;
}
