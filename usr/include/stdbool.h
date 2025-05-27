#ifndef _STDBOOL_H
#define _STDBOOL_H 1

/* Prevent C++ name mangling */
#ifdef __cplusplus
extern "C" {
#endif

/* Check if we're using a C99 or later compiler */
#if !defined(__STDC_VERSION__) || (__STDC_VERSION__ < 199901L)
    #if !defined(__cplusplus)
        #warning "This header requires C99 or later."
    #endif
#endif

/* Handle GCC-specific attributes and optimizations */
#ifdef __GNUC__
    #define _BOOL_INLINE __inline__ __attribute__((always_inline))
    #define _BOOL_UNUSED __attribute__((unused))
#else
    #define _BOOL_INLINE
    #define _BOOL_UNUSED
#endif

/* Ensure proper bool type definition */
#ifndef __cplusplus
    #if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
        /* C99 or later - use _Bool as per standard */
        #define bool _Bool
    #else
        /* Pre-C99 compatibility */
        typedef unsigned char bool;
    #endif
#endif

/* Define standard boolean constants */
#define true 1
#define false 0
#define __bool_true_false_are_defined 1

/* Additional safety and convenience macros */
#define BOOL_TO_STR(x) ((x) ? "true" : "false")
#define IS_BOOL(x) __builtin_types_compatible_p(typeof(x), bool)

/* Boolean validation macro (compile-time check for boolean type) */
#define VALIDATE_BOOL(x) \
    _Static_assert(IS_BOOL(x), "Expression must be of type bool")

/* Optimized boolean operations */
static _BOOL_INLINE bool bool_and(bool a, bool b) _BOOL_UNUSED;
static _BOOL_INLINE bool bool_or(bool a, bool b) _BOOL_UNUSED;
static _BOOL_INLINE bool bool_xor(bool a, bool b) _BOOL_UNUSED;
static _BOOL_INLINE bool bool_not(bool a) _BOOL_UNUSED;

static _BOOL_INLINE bool bool_and(bool a, bool b) {
    return (bool)(a && b);
}

static _BOOL_INLINE bool bool_or(bool a, bool b) {
    return (bool)(a || b);
}

static _BOOL_INLINE bool bool_xor(bool a, bool b) {
    return (bool)(a ^ b);
}

static _BOOL_INLINE bool bool_not(bool a) {
    return (bool)(!a);
}

/* Size verification for bool type */
_Static_assert(sizeof(bool) == 1, "Bool must be 1 byte in size");

/* Debug assertion macros (only active in debug builds) */
#ifdef DEBUG
    #define ASSERT_BOOL(x) \
        do { \
            if (!IS_BOOL(x)) { \
                __builtin_trap(); \
            } \
        } while(0)
#else
    #define ASSERT_BOOL(x) ((void)0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* _STDBOOL_H */
