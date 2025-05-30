#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

// Forward declarations for static functions
static size_t partition(char* base, size_t low, size_t high, size_t size,
                      int (*compare)(const void*, const void*));
static void swap_elements(void* a, void* b, size_t size);

// Static variables for memory management and random number generation
static unsigned long next_rand = 1;

// Simple heap management
// This is a basic implementation
extern void* _sbrk(intptr_t increment);  // System call to extend heap
extern char end;        // Provided by linker script
static char* heap_end = &end;

void* _sbrk(ptrdiff_t incr) {
    char* prev_heap_end = heap_end;
    heap_end += incr;
    return (void*) prev_heap_end;
}

// Memory management functions
void* malloc(size_t size) {
    // Basic implementation
    if (size == 0) return NULL;
    
    // Align size to pointer boundary
    size = (size + sizeof(void*) - 1) & ~(sizeof(void*) - 1);
    
    void* ptr = _sbrk(size + sizeof(size_t));
    if (ptr == (void*)-1) return NULL;
    
    *(size_t*)ptr = size;
    return (char*)ptr + sizeof(size_t);
}

void* calloc(size_t num, size_t size) {
    size_t total_size = num * size;
    if (num != 0 && total_size / num != size) return NULL; // Overflow check
    
    void* ptr = malloc(total_size);
    if (ptr) {
        char* p = (char*)ptr;
        for (size_t i = 0; i < total_size; i++) {
            p[i] = 0;
        }
    }
    return ptr;
}

void* realloc(void* ptr, size_t size) {
    if (!ptr) return malloc(size);
    if (size == 0) {
        free(ptr);
        return NULL;
    }
    
    size_t old_size = *((size_t*)((char*)ptr - sizeof(size_t)));
    void* new_ptr = malloc(size);
    if (!new_ptr) return NULL;
    
    size_t copy_size = (old_size < size) ? old_size : size;
    char* src = (char*)ptr;
    char* dst = (char*)new_ptr;
    for (size_t i = 0; i < copy_size; i++) {
        dst[i] = src[i];
    }
    
    free(ptr);
    return new_ptr;
}

int64_t __divdi3(int64_t a, int64_t b) {
    return a / b;
}

int64_t __moddi3(int64_t a, int64_t b) {
    return a % b;
}

void free(void* ptr) {
    (void)ptr; // Mark as unused to suppress warning
    // In a real implementation, you'd add this block back to free list
}

// Process control functions
void abort(void) {
    // Should terminate program abnormally
    _Exit(EXIT_FAILURE);
}

void exit(int status) {
    _Exit(status);
}

void _Exit(int status) {
    (void)status; // Mark as unused
    while (1) {} // Infinite loop as placeholder
}

static void (*atexit_funcs[32])(void);
static int atexit_count = 0;

int atexit(void (*function)(void)) {
    if (atexit_count >= 32 || !function) return -1;
    atexit_funcs[atexit_count++] = function;
    return 0;
}

// String conversion functions
double atof(const char* str) {
    return strtod(str, NULL);
}

int atoi(const char* str) {
    int result = 0;
    int sign = 1;
    int i = 0;
    
    // Skip whitespace
    while (str[i] == ' ' || str[i] == '\t' || str[i] == '\n' || 
           str[i] == '\r' || str[i] == '\f' || str[i] == '\v') {
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
    
    return sign * result;
}

long atol(const char* str) {
    return strtol(str, NULL, 10);
}

long long atoll(const char* str) {
    return strtoll(str, NULL, 10);
}

double strtod(const char* str, char** endptr) {
    // Simplified implementation - full IEEE754 parsing is complex
    double result = 0.0;
    double sign = 1.0;
    const char* p = str;
    
    // Skip whitespace
    while (*p == ' ' || *p == '\t' || *p == '\n') p++;
    
    // Handle sign
    if (*p == '-') {
        sign = -1.0;
        p++;
    } else if (*p == '+') {
        p++;
    }
    
    // Parse integer part
    while (*p >= '0' && *p <= '9') {
        result = result * 10.0 + (*p - '0');
        p++;
    }
    
    // Parse fractional part
    if (*p == '.') {
        p++;
        double fraction = 0.0;
        double divisor = 10.0;
        while (*p >= '0' && *p <= '9') {
            fraction += (*p - '0') / divisor;
            divisor *= 10.0;
            p++;
        }
        result += fraction;
    }
    
    if (endptr) *endptr = (char*)p;
    return sign * result;
}

float strtof(const char* str, char** endptr) {
    return (float)strtod(str, endptr);
}

long double strtold(const char* str, char** endptr) {
    return (long double)strtod(str, endptr);
}

long strtol(const char* str, char** endptr, int base) {
    long result = 0;
    int sign = 1;
    const char* p = str;
    
    // Skip whitespace
    while (*p == ' ' || *p == '\t' || *p == '\n') p++;
    
    // Handle sign
    if (*p == '-') {
        sign = -1;
        p++;
    } else if (*p == '+') {
        p++;
    }
    
    // Auto-detect base
    if (base == 0) {
        if (*p == '0') {
            if (p[1] == 'x' || p[1] == 'X') {
                base = 16;
                p += 2;
            } else {
                base = 8;
            }
        } else {
            base = 10;
        }
    } else if (base == 16 && *p == '0' && (p[1] == 'x' || p[1] == 'X')) {
        p += 2;
    }
    
    while (*p) {
        int digit;
        if (*p >= '0' && *p <= '9') {
            digit = *p - '0';
        } else if (*p >= 'a' && *p <= 'z') {
            digit = *p - 'a' + 10;
        } else if (*p >= 'A' && *p <= 'Z') {
            digit = *p - 'A' + 10;
        } else {
            break;
        }
        
        if (digit >= base) break;
        result = result * base + digit;
        p++;
    }
    
    if (endptr) *endptr = (char*)p;
    return sign * result;
}

long long strtoll(const char* str, char** endptr, int base) {
    // Similar to strtol but for long long
    long long result = 0;
    int sign = 1;
    const char* p = str;
    
    while (*p == ' ' || *p == '\t' || *p == '\n') p++;
    
    if (*p == '-') {
        sign = -1;
        p++;
    } else if (*p == '+') {
        p++;
    }
    
    if (base == 0) {
        if (*p == '0') {
            if (p[1] == 'x' || p[1] == 'X') {
                base = 16;
                p += 2;
            } else {
                base = 8;
            }
        } else {
            base = 10;
        }
    } else if (base == 16 && *p == '0' && (p[1] == 'x' || p[1] == 'X')) {
        p += 2;
    }
    
    while (*p) {
        int digit;
        if (*p >= '0' && *p <= '9') {
            digit = *p - '0';
        } else if (*p >= 'a' && *p <= 'z') {
            digit = *p - 'a' + 10;
        } else if (*p >= 'A' && *p <= 'Z') {
            digit = *p - 'A' + 10;
        } else {
            break;
        }
        
        if (digit >= base) break;
        result = result * base + digit;
        p++;
    }
    
    if (endptr) *endptr = (char*)p;
    return sign * result;
}

unsigned long strtoul(const char* str, char** endptr, int base) {
    return (unsigned long)strtol(str, endptr, base);
}

unsigned long long strtoull(const char* str, char** endptr, int base) {
    return (unsigned long long)strtoll(str, endptr, base);
}

// Integer to string conversion
void itoa(int num, char* str, int base) {
    int i = 0;
    bool isNegative = false;
    
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }
    
    if (num < 0 && base == 10) {
        isNegative = true;
        num = -num;
    }
    
    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }
    
    if (isNegative) {
        str[i++] = '-';
    }
    
    str[i] = '\0';
    
    // Reverse string
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

void ltoa(long num, char* str, int base) {
    int i = 0;
    bool isNegative = false;
    
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }
    
    if (num < 0 && base == 10) {
        isNegative = true;
        num = -num;
    }
    
    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }
    
    if (isNegative) {
        str[i++] = '-';
    }
    
    str[i] = '\0';
    
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

void lltoa(long long num, char* str, int base) {
    int i = 0;
    bool isNegative = false;
    
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }
    
    if (num < 0 && base == 10) {
        isNegative = true;
        num = -num;
    }
    
    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }
    
    if (isNegative) {
        str[i++] = '-';
    }
    
    str[i] = '\0';
    
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

// Mathematical functions
int abs(int x) {
    return (x < 0) ? -x : x;
}

long labs(long x) {
    return (x < 0) ? -x : x;
}

long long llabs(long long x) {
    return (x < 0) ? -x : x;
}

div_t div(int numerator, int denominator) {
    div_t result;
    result.quot = numerator / denominator;
    result.rem = numerator % denominator;
    return result;
}

ldiv_t ldiv(long numerator, long denominator) {
    ldiv_t result;
    result.quot = numerator / denominator;
    result.rem = numerator % denominator;
    return result;
}

lldiv_t lldiv(long long numerator, long long denominator) {
    lldiv_t result;
    result.quot = numerator / denominator;
    result.rem = numerator % denominator;
    return result;
}

// Random number generation (Linear Congruential Generator)
int rand(void) {
    next_rand = next_rand * 1103515245 + 12345;
    return (unsigned int)(next_rand / 65536) % 32768;
}

void srand(unsigned int seed) {
    next_rand = seed;
}

// Searching and sorting
void* bsearch(const void* key, const void* base, size_t num, size_t size,
              int (*compare)(const void*, const void*)) {
    const char* ptr = (const char*)base;
    size_t low = 0, high = num;
    
    while (low < high) {
        size_t mid = low + (high - low) / 2;
        const void* mid_ptr = ptr + mid * size;
        int cmp = compare(key, mid_ptr);
        
        if (cmp == 0) {
            return (void*)mid_ptr;
        } else if (cmp < 0) {
            high = mid;
        } else {
            low = mid + 1;
        }
    }
    
    return NULL;
}

static void quicksort(char* base, size_t low, size_t high, size_t size,
                     int (*compare)(const void*, const void*)) {
    if (low < high) {
        size_t pi = partition(base, low, high, size, compare);
        if (pi > 0) quicksort(base, low, pi - 1, size, compare);
        quicksort(base, pi + 1, high, size, compare);
    }
}

static size_t partition(char* base, size_t low, size_t high, size_t size,
                       int (*compare)(const void*, const void*)) {
    char* pivot = base + high * size;
    size_t i = low;
    
    for (size_t j = low; j < high; j++) {
        if (compare(base + j * size, pivot) < 0) {
            swap_elements(base + i * size, base + j * size, size);
            i++;
        }
    }
    swap_elements(base + i * size, base + high * size, size);
    return i;
}

static void swap_elements(void* a, void* b, size_t size) {
    char* pa = (char*)a;
    char* pb = (char*)b;
    for (size_t i = 0; i < size; i++) {
        char temp = pa[i];
        pa[i] = pb[i];
        pb[i] = temp;
    }
}

void qsort(void* base, size_t num, size_t size,
           int (*compare)(const void*, const void*)) {
    if (num > 1) {
        quicksort((char*)base, 0, num - 1, size, compare);
    }
}

// Environment functions (basic implementations)
char* getenv(const char* name) {
    (void)name; // Mark as unused
    return NULL; // Placeholder
}

int system(const char* command) {
    (void)command; // Mark as unused
    return -1; // Placeholder
}

// Multibyte/wide character functions (basic implementations)
int mblen(const char* s, size_t n) {
    if (!s) return 0;
    if (!*s) return 0;
    (void)n; // Mark as unused
    return 1; // Assuming single-byte characters
}

int mbtowc(wchar_t* pwc, const char* s, size_t n) {
    if (!s) return 0;
    if (!*s) {
        if (pwc) *pwc = 0;
        return 0;
    }
    (void)n; // Mark as unused
    if (pwc) *pwc = (unsigned char)*s;
    return 1;
}

int wctomb(char* s, wchar_t wchar) {
    if (!s) return 0;
    if (wchar > 255) return -1;
    *s = (char)wchar;
    return 1;
}

size_t mbstowcs(wchar_t* dest, const char* src, size_t max) {
    size_t count = 0;
    while (count < max && *src) {
        if (dest) dest[count] = (unsigned char)*src;
        src++;
        count++;
    }
    if (count < max && dest) dest[count] = 0;
    return count;
}

size_t wcstombs(char* dest, const wchar_t* src, size_t max) {
    size_t count = 0;
    while (count < max && *src) {
        if (*src > 255) return (size_t)-1;
        if (dest) dest[count] = (char)*src;
        src++;
        count++;
    }
    if (count < max && dest) dest[count] = 0;
    return count;
}