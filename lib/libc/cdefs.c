#include <sys/cdefs.h>

/* 
 * This file is intentionally left mostly empty since cdefs.h primarily contains
 * macros and compiler directives rather than actual code implementations.
 *
 * However, we can include some architecture-specific definitions or compatibility
 * implementations here if needed.
 */

/* Architecture-specific byte order handling */
#if __BYTE_ORDER == __LITTLE_ENDIAN
const int __os_endian = 0;
#else
const int __os_endian = 1;
#endif

/* Weak symbol default implementations */
__weak void __assert_fail(const char *expr, const char *file, 
                         int line, const char *func) {
    (void)expr;  // Silence unused parameter warning
    (void)file;  // Silence unused parameter warning
    (void)line;  // Silence unused parameter warning
    (void)func;  // Silence unused parameter warning
    
    /* Default implementation for assertion failures */
    while (1) {}  // Hang on assertion failure
}

/* Compiler barrier implementation */
void __compiler_barrier(void) {
    __asm__ __volatile__("" : : : "memory");
}

/* Builtin function stubs for non-GCC compilers */
#if !__GNUC_PREREQ(3,0)
void *__builtin_memcpy(void *dest, const void *src, size_t n) {
    char *d = dest;
    const char *s = src;
    while (n--) *d++ = *s++;
    return dest;
}

void *__builtin_memset(void *s, int c, size_t n) {
    char *p = s;
    while (n--) *p++ = c;
    return s;
}
#endif
