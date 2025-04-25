#ifndef STDLIB_H
#define STDLIB_H

#include <stdint.h>

#define EXIT_SUCCESS 0;
#define EXIT_FAILURE 1;

typedef struct {
    int quot;
    int rem;
} div_t;

// Converts an integer to a string (itoa)
void itoa(int num, char* str, int base);

// Converts a string to an integer (atoi)
int atoi(const char* str);

// Returns the absolute value of an integer
int abs(int x);

// Returns the quotient and remainder of a division
div_t div(int numerator, int denominator);

#endif
