#ifndef _STDINT_H
#define _STDINT_H 1

/* Prevent C++ name mangling */
#ifdef __cplusplus
extern "C" {
#endif

/* GCC specific attributes */
#ifdef __GNUC__
    #define _INT_UNUSED __attribute__((unused))
    #define _INT_PACKED __attribute__((packed))
    #define _INT_ALIGNED(x) __attribute__((aligned(x)))
#else
    #define _INT_UNUSED
    #define _INT_PACKED
    #define _INT_ALIGNED(x)
#endif

/* Exact-width integer types */
typedef signed char        int8_t;
typedef unsigned char      uint8_t;
typedef signed short      int16_t;
typedef unsigned short    uint16_t;
typedef signed int        int32_t;
typedef unsigned int      uint32_t;
typedef signed long long  int64_t;
typedef unsigned long long uint64_t;

/* Verify sizes at compile time */
_Static_assert(sizeof(int8_t)  == 1, "int8_t must be 1 byte");
_Static_assert(sizeof(uint8_t) == 1, "uint8_t must be 1 byte");
_Static_assert(sizeof(int16_t) == 2, "int16_t must be 2 bytes");
_Static_assert(sizeof(uint16_t)== 2, "uint16_t must be 2 bytes");
_Static_assert(sizeof(int32_t) == 4, "int32_t must be 4 bytes");
_Static_assert(sizeof(uint32_t)== 4, "uint32_t must be 4 bytes");
_Static_assert(sizeof(int64_t) == 8, "int64_t must be 8 bytes");
_Static_assert(sizeof(uint64_t)== 8, "uint64_t must be 8 bytes");

/* Minimum-width integer types */
typedef int8_t    int_least8_t;
typedef uint8_t   uint_least8_t;
typedef int16_t   int_least16_t;
typedef uint16_t  uint_least16_t;
typedef int32_t   int_least32_t;
typedef uint32_t  uint_least32_t;
typedef int64_t   int_least64_t;
typedef uint64_t  uint_least64_t;

/* Fastest minimum-width integer types */
typedef int32_t   int_fast8_t;
typedef uint32_t  uint_fast8_t;
typedef int32_t   int_fast16_t;
typedef uint32_t  uint_fast16_t;
typedef int32_t   int_fast32_t;
typedef uint32_t  uint_fast32_t;
typedef int64_t   int_fast64_t;
typedef uint64_t  uint_fast64_t;

/* Integer type capable of holding object pointers */
typedef int32_t   intptr_t;
typedef uint32_t  uintptr_t;

/* Greatest-width integer types */
typedef int64_t   intmax_t;
typedef uint64_t  uintmax_t;

/* Limits of exact-width integer types */
#define INT8_MIN   (-128)
#define INT8_MAX   127
#define UINT8_MAX  255

#define INT16_MIN  (-32768)
#define INT16_MAX  32767
#define UINT16_MAX 65535

#define INT32_MIN  (-2147483648)
#define INT32_MAX  2147483647
#define UINT32_MAX 4294967295U

#define INT64_MIN  (-9223372036854775808LL)
#define INT64_MAX  9223372036854775807LL
#define UINT64_MAX 18446744073709551615ULL

/* Limits of minimum-width integer types */
#define INT_LEAST8_MIN   INT8_MIN
#define INT_LEAST8_MAX   INT8_MAX
#define UINT_LEAST8_MAX  UINT8_MAX

#define INT_LEAST16_MIN  INT16_MIN
#define INT_LEAST16_MAX  INT16_MAX
#define UINT_LEAST16_MAX UINT16_MAX

#define INT_LEAST32_MIN  INT32_MIN
#define INT_LEAST32_MAX  INT32_MAX
#define UINT_LEAST32_MAX UINT32_MAX

#define INT_LEAST64_MIN  INT64_MIN
#define INT_LEAST64_MAX  INT64_MAX
#define UINT_LEAST64_MAX UINT64_MAX

/* Limits of fastest minimum-width integer types */
#define INT_FAST8_MIN    INT32_MIN
#define INT_FAST8_MAX    INT32_MAX
#define UINT_FAST8_MAX   UINT32_MAX

#define INT_FAST16_MIN   INT32_MIN
#define INT_FAST16_MAX   INT32_MAX
#define UINT_FAST16_MAX  UINT32_MAX

#define INT_FAST32_MIN   INT32_MIN
#define INT_FAST32_MAX   INT32_MAX
#define UINT_FAST32_MAX  UINT32_MAX

#define INT_FAST64_MIN   INT64_MIN
#define INT_FAST64_MAX   INT64_MAX
#define UINT_FAST64_MAX  UINT64_MAX

/* Limits of integer types capable of holding object pointers */
#define INTPTR_MIN   INT32_MIN
#define INTPTR_MAX   INT32_MAX
#define UINTPTR_MAX  UINT32_MAX

/* Limits of greatest-width integer types */
#define INTMAX_MIN   INT64_MIN
#define INTMAX_MAX   INT64_MAX
#define UINTMAX_MAX  UINT64_MAX

/* Limits of other integer types */
#define PTRDIFF_MIN INT32_MIN
#define PTRDIFF_MAX INT32_MAX

#define SIZE_MAX    UINT32_MAX

/* Macros for integer constants */
#define INT8_C(x)    (x)
#define UINT8_C(x)   (x)
#define INT16_C(x)   (x)
#define UINT16_C(x)  (x)
#define INT32_C(x)   (x)
#define UINT32_C(x)  (x##U)
#define INT64_C(x)   (x##LL)
#define UINT64_C(x)  (x##ULL)
#define INTMAX_C(x)  (x##LL)
#define UINTMAX_C(x) (x##ULL)

/* Additional utility macros */
#define IS_POWER_OF_2(x) (((x) != 0) && ((x) & ((x) - 1)) == 0)

/* Alignment macros */
#define ALIGN_UP(x, align)   (((x) + ((align) - 1)) & ~((align) - 1))
#define ALIGN_DOWN(x, align) ((x) & ~((align) - 1))

/* Byte order conversion (assuming little-endian architecture) */
static inline uint16_t _INT_UNUSED swap16(uint16_t x) {
    return (x << 8) | (x >> 8);
}

static inline uint32_t _INT_UNUSED swap32(uint32_t x) {
    return ((x << 24) & 0xFF000000) |
           ((x <<  8) & 0x00FF0000) |
           ((x >>  8) & 0x0000FF00) |
           ((x >> 24) & 0x000000FF);
}

static inline uint64_t _INT_UNUSED swap64(uint64_t x) {
    return ((x << 56) & 0xFF00000000000000ULL) |
           ((x << 40) & 0x00FF000000000000ULL) |
           ((x << 24) & 0x0000FF0000000000ULL) |
           ((x <<  8) & 0x000000FF00000000ULL) |
           ((x >>  8) & 0x00000000FF000000ULL) |
           ((x >> 24) & 0x0000000000FF0000ULL) |
           ((x >> 40) & 0x000000000000FF00ULL) |
           ((x >> 56) & 0x00000000000000FFULL);
}

/* Debugging macros (active when DEBUG is defined) */
#ifdef DEBUG
    #define ASSERT_POWER_OF_2(x) \
        _Static_assert(IS_POWER_OF_2(x), "Value must be power of 2")
    
    #define VERIFY_ALIGNMENT(ptr, align) \
        do { \
            if ((uintptr_t)(ptr) % (align) != 0) { \
                __builtin_trap(); \
            } \
        } while(0)
#else
    #define ASSERT_POWER_OF_2(x) ((void)0)
    #define VERIFY_ALIGNMENT(ptr, align) ((void)0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* _STDINT_H */
