#include <stdio.h>
#include <string.h>
#include <limits.h>

int cowsay_main(int argc, char **argv) {
    if (argc == 1) {
        printf("Usage: cowsay [message]\n");
        return 0;
    }

    // Use size_t instead of int for length
    size_t length = 0;
    for (int i = 1; i < argc; i++) {
        size_t arg_len = strlen(argv[i]);
        if (length > SIZE_MAX - arg_len) {  // Check for overflow
            fprintf(stderr, "Error: message too long\n");
            return 1;
        }
        length += arg_len;
        if (i < argc - 1) {
            if (length == SIZE_MAX) {  // No space left for the extra space
                fprintf(stderr, "Error: message too long\n");
                return 1;
            }
            length++; // for space
        }
    }

    // Rest of the code remains the same...
    // Print top border
    printf(" ");
    for (size_t i = 0; i < length + 2; i++) printf("_");
    printf("\n");

    // Print message
    printf("< ");
    for (int i = 1; i < argc; i++) {
        printf("%s", argv[i]);
        if (i < argc - 1) printf(" ");
    }
    printf(" >\n");

    // Print bottom border
    printf(" ");
    for (size_t i = 0; i < length + 2; i++) printf("-");
    printf("\n");

    // Print the cow
    printf("        \\   ^__^\n");
    printf("         \\  (oo)\\_______\n");
    printf("            (__)\\       )\\/\\\n");
    printf("                ||----w |\n");
    printf("                ||     ||\n");

    return 0;
}
