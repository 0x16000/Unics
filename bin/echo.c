#include <stdio.h>
#include <string.h>

int echo_main(int argc, const char **argv) {
    int skip_newline = 0;
    
    // Handle -n flag
    if (argc > 1 && strcmp(argv[1], "-n") == 0) {
        skip_newline = 1;
        argc--;
        argv++;
    }

    // Print all arguments with spaces
    for (int i = 1; i < argc; i++) {
        printf("%s", argv[i]);
        if (i < argc - 1) {
            printf(" ");
        }
    }

    // Print newline unless -n was specified
    if (!skip_newline) {
        printf("\n");
    }
    
    return 0;
}
