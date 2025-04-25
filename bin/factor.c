#include <stdio.h>
#include <string.h>

int factor_main(int argc, char **argv) {
    int n;

    // Check if an argument is provided
    if (argc < 2) {
        printf("Usage: factor <number>\n");
        return 1;
    }

    // Parse the number from the argument
    sscanf(argv[1], "%d", &n);

    // Handle the case for 0 and 1 (no prime factors)
    if (n <= 1) {
        printf("No prime factors for %d\n", n);
        return 0;
    }

    printf("Prime factors of %d are: ", n);
    
    // Print all factors of 2
    while (n % 2 == 0) {
        printf("2 ");
        n = n / 2;
    }

    // Print odd factors
    for (int i = 3; i * i <= n; i += 2) {
        while (n % i == 0) {
            printf("%d ", i);
            n = n / i;
        }
    }

    // If n is a prime number greater than 2
    if (n > 2) {
        printf("%d", n);
    }

    printf("\n");
    return 0;
}

