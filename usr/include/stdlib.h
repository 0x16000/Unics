#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

extern void* _sbrk(intptr_t incr);

// Exit codes
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

// Maximum random number returned by rand()
#define RAND_MAX 32767

// Maximum number of bytes in a multibyte character
#define MB_CUR_MAX 1

// Division result structures
typedef struct {
    int quot;
    int rem;
} div_t;

typedef struct {
    long quot;
    long rem;
} ldiv_t;

typedef struct {
    long long quot;
    long long rem;
} lldiv_t;

// Memory management functions
void* malloc(size_t size);
void* calloc(size_t num, size_t size);
void* realloc(void* ptr, size_t size);
void free(void* ptr);

// Process control functions
void abort(void);
void exit(int status);
void _Exit(int status);
int atexit(void (*function)(void));

// String conversion functions
double atof(const char* str);
int atoi(const char* str);
long atol(const char* str);
long long atoll(const char* str);

double strtod(const char* str, char** endptr);
float strtof(const char* str, char** endptr);
long double strtold(const char* str, char** endptr);

long strtol(const char* str, char** endptr, int base);
long long strtoll(const char* str, char** endptr, int base);
unsigned long strtoul(const char* str, char** endptr, int base);
unsigned long long strtoull(const char* str, char** endptr, int base);

// Integer to string conversion (non-standard but commonly used)
void itoa(int num, char* str, int base);
void ltoa(long num, char* str, int base);
void lltoa(long long num, char* str, int base);

// Mathematical functions
int abs(int x);
long labs(long x);
long long llabs(long long x);

div_t div(int numerator, int denominator);
ldiv_t ldiv(long numerator, long denominator);
lldiv_t lldiv(long long numerator, long long denominator);

// Random number generation
int rand(void);
void srand(unsigned int seed);

// Searching and sorting
void* bsearch(const void* key, const void* base, size_t num, size_t size,
              int (*compare)(const void*, const void*));
void qsort(void* base, size_t num, size_t size,
           int (*compare)(const void*, const void*));

// Environment functions
char* getenv(const char* name);
int system(const char* command);

// Multibyte/wide character conversion
int mblen(const char* s, size_t n);
int mbtowc(wchar_t* pwc, const char* s, size_t n);
int wctomb(char* s, wchar_t wchar);
size_t mbstowcs(wchar_t* dest, const char* src, size_t max);
size_t wcstombs(char* dest, const wchar_t* src, size_t max);

#endif /* STDLIB_H */