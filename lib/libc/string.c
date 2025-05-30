#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Calculate the length of a string
size_t strlen(const char *str) {
    const char *s = str;
    while (*s) s++;
    return s - str;
}

// Compare two strings
int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

// Compare two strings up to n characters
int strncmp(const char *s1, const char *s2, size_t n) {
    if (n == 0) return 0;
    while (--n && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

// Copy a string
char *strcpy(char *dest, const char *src) {
    char *ret = dest;
    while ((*dest++ = *src++));
    return ret;
}

// Copy up to n characters from a string
char *strncpy(char *dest, const char *src, size_t n) {
    char *ret = dest;
    while (n && (*dest++ = *src++)) n--;
    while (n--) *dest++ = '\0';
    return ret;
}

// Concatenate two strings
char *strcat(char *dest, const char *src) {
    char *ret = dest;
    while (*dest) dest++;
    while ((*dest++ = *src++));
    return ret;
}

// Concatenate up to n characters from a string
char *strncat(char *dest, const char *src, size_t n) {
    char *ret = dest;
    while (*dest) dest++;
    while (n-- && (*dest++ = *src++));
    *dest = '\0';
    return ret;
}

// Set n bytes of memory to a specific value
void *memset(void *s, int c, size_t n) {
    unsigned char *p = s;
    while (n--) *p++ = (unsigned char)c;
    return s;
}

// Copy n bytes from one memory location to another
void *memcpy(void *dest, const void *src, size_t n) {
    unsigned char *d = dest;
    const unsigned char *s = src;
    while (n--) *d++ = *s++;
    return dest;
}

// Move n bytes from one memory location to another, handling overlap
void *memmove(void *dest, const void *src, size_t n) {
    unsigned char *d = dest;
    const unsigned char *s = src;
    if (d < s) {
        while (n--) *d++ = *s++;
    } else {
        d += n;
        s += n;
        while (n--) *--d = *--s;
    }
    return dest;
}

// Compare n bytes of memory
int memcmp(const void *s1, const void *s2, size_t n) {
    const unsigned char *p1 = s1, *p2 = s2;
    while (n--) {
        if (*p1 != *p2) return *p1 - *p2;
        p1++;
        p2++;
    }
    return 0;
}

// Find the first occurrence of a character in a string
char *strchr(const char *s, int c) {
    while (*s != (char)c) {
        if (!*s++) return NULL;
    }
    return (char *)s;
}

// Find the last occurrence of a character in a string
char *strrchr(const char *s, int c) {
    const char *last = NULL;
    do {
        if (*s == (char)c) last = s;
    } while (*s++);
    return (char *)last;
}

// Find the first occurrence of a substring in a string
char *strstr(const char *haystack, const char *needle) {
    size_t needle_len = strlen(needle);
    if (needle_len == 0) return (char *)haystack;
    while (*haystack) {
        if (*haystack == *needle && !strncmp(haystack, needle, needle_len)) {
            return (char *)haystack;
        }
        haystack++;
    }
    return NULL;
}

// Find the length of the initial segment of a string consisting of characters not in a specified set
size_t strcspn(const char *s, const char *reject) {
    size_t count = 0;
    while (*s) {
        if (strchr(reject, *s)) break;
        s++;
        count++;
    }
    return count;
}

// Find the length of the initial segment of a string consisting of characters in a specified set
size_t strspn(const char *s, const char *accept) {
    size_t count = 0;
    while (*s && strchr(accept, *s)) {
        s++;
        count++;
    }
    return count;
}

// Tokenize a string
char *strtok(char *str, const char *delim) {
    static char *last_token = NULL;
    char *start, *end;

    if (str != NULL) {
        last_token = str;
    }

    if (last_token == NULL || *last_token == '\0') {
        return NULL;
    }

    // Skip leading delimiters
    start = last_token;
    while (*start && strchr(delim, *start)) {
        start++;
    }

    if (*start == '\0') {
        last_token = NULL;
        return NULL;
    }

    // Find the end of the token
    end = start;
    while (*end && !strchr(delim, *end)) {
        end++;
    }

    if (*end == '\0') {
        last_token = NULL;
    } else {
        *end = '\0';
        last_token = end + 1;
    }

    return start;
}