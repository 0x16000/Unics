#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Evaluate simple expression: tokens is an array of strings like {"3", "+", "4", "*", "2"}
// No operator precedence, evaluates strictly left to right.
static int eval_expr(int argc, char **argv, int *result) {
    if (argc < 1) return -1;

    int res = atoi(argv[0]);
    int i = 1;

    while (i + 1 < argc) {
        char *op = argv[i];
        int val = atoi(argv[i + 1]);

        if (strcmp(op, "+") == 0) {
            res += val;
        } else if (strcmp(op, "-") == 0) {
            res -= val;
        } else if (strcmp(op, "*") == 0) {
            res *= val;
        } else if (strcmp(op, "/") == 0) {
            if (val == 0) {
                fprintf(stderr, "bc: division by zero\n");
                return -1;
            }
            res /= val;
        } else {
            fprintf(stderr, "bc: unknown operator '%s'\n", op);
            return -1;
        }
        i += 2;
    }

    *result = res;
    return 0;
}

int bc_main(int argc, char **argv) {
    if (argc > 1) {
        // Evaluate expression passed as command line arguments
        int result = 0;
        if (eval_expr(argc - 1, argv + 1, &result) == 0) {
            printf("%d\n", result);
            return 0;
        } else {
            return 1;
        }
    } else {
        // Interactive mode: immediate echo input character by character
        printf("bc simple calculator. Type 'quit' or Ctrl-D to exit.\n");
        printf("> ");

        char expr[256];
        int pos = 0;
        int c;

        while ((c = getchar()) != EOF) {
            putchar(c);  // echo char immediately

            if (c == '\n') {
                expr[pos] = '\0';
                pos = 0;

                if (strcmp(expr, "quit") == 0)
                    break;

                if (expr[0] == '\0') {
                    printf("> ");
                    continue;
                }

                // Tokenize input line by spaces
                char *tokens[64];
                int token_count = 0;
                char *token = strtok(expr, " ");
                while (token && token_count < 64) {
                    tokens[token_count++] = token;
                    token = strtok(NULL, " ");
                }

                int result = 0;
                if (eval_expr(token_count, tokens, &result) == 0) {
                    printf("%d\n", result);
                } else {
                    printf("Error evaluating expression.\n");
                }

                printf("> ");
            } else {
                if (pos < (int)(sizeof(expr) - 1)) {
                    expr[pos++] = (char)c;
                }
                // ignore extra chars if line too long
            }
        }

        printf("\nGoodbye.\n");
        return 0;
    }
}
