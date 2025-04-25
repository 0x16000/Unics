#include <stdio.h>
#include <string.h>

int cowsay_main(int argc, char **argv) {
    // If no arguments, show usage
    if (argc == 1) {
        printf("Usage: cowsay [message]\n");
        return 0;
    }

    // Calculate message length
    int length = 0;
    for (int i = 1; i < argc; i++) {
        length += strlen(argv[i]);
        if (i < argc - 1) length++; // for space
    }

    // Print top border
    printf(" ");
    for (int i = 0; i < length + 2; i++) printf("_");
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
    for (int i = 0; i < length + 2; i++) printf("-");
    printf("\n");

    // Print the cow
    printf("        \\   ^__^\n");
    printf("         \\  (oo)\\_______\n");
    printf("            (__)\\       )\\/\\\n");
    printf("                ||----w |\n");
    printf("                ||     ||\n");

    return 0;
}
