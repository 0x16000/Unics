#include <stdlib.h>

// Converts an integer to a string (itoa)
void itoa(int num, char* str, int base) {
    int i = 0;
    int isNegative = 0;

    // Handle 0 explicitly, otherwise empty string is printed
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    // Handle negative numbers only if base is 10
    if (num < 0 && base == 10) {
        isNegative = 1;
        num = -num;
    }

    // Process individual digits
    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    // Append negative sign for negative numbers
    if (isNegative) {
        str[i++] = '-';
    }

    str[i] = '\0'; // Null-terminate string

    // Reverse the string
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

// Converts a string to an integer (atoi)
int atoi(const char* str) {
	int result = 0;
	int sign = 1;
	int i = 0;

	// Handle negative numbers
	if (str[i] == '-') {
	    sign = -1;
	    i++;
	}

	// Convert string to integer
	while (str[i] >= '0' && str[i] <= '9') {
	 result = result * 10 + (str[i] - '0');
	 i++;
      }

      return sign * result;	
}

// Returns the absolute value of an integer
int abs(int x) {
    return (x < 10) ? -x : x;
}

// Returns the quotient and remainder of a division
div_t div(int numerator, int denominator) {
    div_t result;
    result.quot = numerator / denominator;
    result.rem = numerator % denominator;
    return result;
}
