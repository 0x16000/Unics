#include <stdio.h>
#include <string.h>

// Simple atoi implementation
int my_atoi(const char *str) {
    int result = 0;
    int sign = 1;
    int i = 0;

    // Handle leading spaces
    while (str[i] == ' ' || str[i] == '\t') {
        i++;
    }

    // Handle sign
    if (str[i] == '-') {
        sign = -1;
        i++;
    } else if (str[i] == '+') {
        i++;
    }

    // Convert digits
    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10 + (str[i] - '0');
        i++;
    }

    return result * sign;
}

// Function to perform the calculation
int calculate(int num1, int num2, char operator) {
    switch (operator) {
        case '+': return num1 + num2;
        case '-': return num1 - num2;
        case '*': return num1 * num2;
        case '/':
            if (num2 == 0) {
                printf("expr: division by zero\n");
                return 0;
            }
            return num1 / num2;
        default:
            printf("expr: invalid operator\n");
            return 0;
    }
}

// Function to print an integer (similar to printf)
void print_int(int num) {
    printf("%d", num);
}

int expr_main(int argc, char **argv) {
    if (argc != 4) {
        printf("Usage: expr <num1> <operator> <num2>\n");
        printf("  Operators: +, -, *, /\n");
        return 1;
    }

    int num1 = my_atoi(argv[1]);
    int num2 = my_atoi(argv[3]);
    char operator = argv[2][0]; // Get the first char of the operator argument

    // Basic error checking for the operator
    if (strlen(argv[2]) != 1 || (operator != '+' && operator != '-' && operator != '*' && operator != '/')) {
        printf("expr: invalid operator\n");
        return 1;
    }
    
    int result = calculate(num1, num2, operator);
    print_int(result);  // Print the result using custom print_int
    printf("\n");  // Print newline after the result
    return 0;
}
