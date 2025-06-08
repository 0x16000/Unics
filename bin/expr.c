#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>

int calculate(int num1, int num2, char operator) {
    switch (operator) {
        case '+': return num1 + num2;
        case '-': return num1 - num2;
        case '*': return num1 * num2;
        case '/':
            if (num2 == 0) {
                fprintf(stderr, "expr: division by zero\n");
                return 0;
            }
            return num1 / num2;
        default:
            fprintf(stderr, "expr: invalid operator\n");
            return 0;
    }
}

int expr_main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: expr <num1> <operator> <num2>\n");
        fprintf(stderr, "  Operators: +, -, *, / (integer division)\n");
        return EXIT_FAILURE;
    }

    // Parse num1
    char *endptr;
    long num1 = strtol(argv[1], &endptr, 10);
    if (*endptr != '\0' || num1 < INT_MIN || num1 > INT_MAX) {
        fprintf(stderr, "expr: invalid number: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    // Parse num2
    long num2 = strtol(argv[3], &endptr, 10);
    if (*endptr != '\0' || num2 < INT_MIN || num2 > INT_MAX) {
        fprintf(stderr, "expr: invalid number: %s\n", argv[3]);
        return EXIT_FAILURE;
    }

    // Validate operator
    if (strlen(argv[2]) != 1 || strchr("+-*/", argv[2][0]) == NULL) {
        fprintf(stderr, "expr: invalid operator: %s\n", argv[2]);
        return EXIT_FAILURE;
    }
    char operator = argv[2][0];

    int result = calculate((int)num1, (int)num2, operator);
    printf("%d\n", result);
    return EXIT_SUCCESS;
}
