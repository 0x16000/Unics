#include <stdio.h>

int tty_main(int argc, char **argv __attribute__((unused))) {
    // Check if there are any arguments, otherwise print the terminal path
    if (argc > 1) {
        // You could add handling for any unexpected arguments
        printf("Usage: tty\n");
        return 1;  // Indicate an error if arguments are provided
    }

    // Print the path of the terminal
    printf("/dev/shell/\n");

    return 0;  // Return success
}
