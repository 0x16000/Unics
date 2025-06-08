#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int factor_main(int argc, char **argv) {
    int n;

    if (argc < 2) {
        printf("Usage: factor <number>\n");
        return 1;
    }

    // Better input validation
    if (sscanf(argv[1], "%d", &n) != 1) {
        printf("Error: '%s' is not a valid number\n", argv[1]);
        return 1;
    }

    // Handle negative numbers
    if (n < 0) {
        printf("Negative number entered. Using absolute value.\n");
        n = -n;
    }

    if (n <= 1) {
        printf("No prime factors for %d\n", n);
        return 0;
    }

    printf("Prime factors of %d are: ", n);
    
    // Track if we've printed any factors (for space management)
    int printed = 0;
    
    // Handle 2 separately
    while (n % 2 == 0) {
        printf("%s2", printed ? " " : "");
        printed = 1;
        n /= 2;
    }

    // Check odd factors up to sqrt(n)
    for (int i = 3; i * i <= n; i += 2) {
        while (n % i == 0) {
            printf("%s%d", printed ? " " : "", i);
            printed = 1;
            n /= i;
        }
    }

    // Handle remaining prime factor
    if (n > 1) {
        printf("%s%d", printed ? " " : "", n);
    }

    printf("\n");
    return 0;
}
