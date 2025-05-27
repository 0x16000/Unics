ifndef _TYPES_H
#define _TYPES_H

#include <stdint.h>
#include <stddef.h>

/* Boolean type definitions */
typedef int32_t bool;
#define true  1
#define false 0

/* Architecture-specific types */
#if defined(__x86_64__) || defined(__aarch64__)
    typedef uint64_t    uintptr_t;   /* Pointer-sized unsigned integer (64-bit) */
    typedef int64_t     intptr_t;    /* Pointer-sized signed integer (64-bit) */
    typedef uint64_t    size_t;      /* Size of objects (64-bit) */
    typedef int64_t     ssize_t;     /* Signed size type (64-bit) */
#else
    typedef uint32_t    uintptr_t;   /* Pointer-sized unsigned integer (32-bit) */
    typedef int32_t     intptr_t;    /* Pointer-sized signed integer (32-bit) */
    typedef uint32_t    size_t;      /* Size of objects (32-bit) */
    typedef int32_t     ssize_t;     /* Signed size type (32-bit) */
#endif

/* Standard integer types (already defined in stdint.h but for clarity) */
typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;

typedef int8_t    s8;
typedef int16_t   s16;
typedef int32_t   s32;
typedef int64_t   s64;

/* Fixed-width types (32-bit/64-bit integers) */
typedef int32_t   int32;
typedef int64_t   int64;
typedef uint32_t  uint32;
typedef uint64_t  uint64;

/* Minimum-width types */
typedef int_least8_t  int8_least;
typedef int_least16_t int16_least;
typedef int_least32_t int32_least;
typedef int_least64_t int64_least;

typedef uint_least8_t  uint8_least;
typedef uint_least16_t uint16_least;
typedef uint_least32_t uint32_least;
typedef uint_least64_t uint64_least;

/* Fastest integer types */
typedef int_fast8_t    int8_fast;
typedef int_fast16_t   int16_fast;
typedef int_fast32_t   int32_fast;
typedef int_fast64_t   int64_fast;

typedef uint_fast8_t   uint8_fast;
typedef uint_fast16_t  uint16_fast;
typedef uint_fast32_t  uint32_fast;
typedef uint_fast64_t  uint64_fast;

/* Pointers */
typedef intptr_t   ptr_t;      /* Pointer-sized signed integer */
typedef uintptr_t  uptr_t;     /* Pointer-sized unsigned integer */

/* 32-bit/64-bit specific types */
typedef uint32_t   phys_addr_t;   /* Physical address */
typedef uint64_t   virt_addr_t;   /* Virtual address */
typedef uint32_t   irq_t;         /* Interrupt request number */
typedef uint32_t   pid_t;         /* Process ID */

/* Page size and alignments */
#define PAGE_SIZE       4096  /* 4 KB page size */
#define PAGE_SHIFT      12    /* Number of bits to shift for 4 KB pages */

/* Type for time and date */
typedef int64_t   time_t;    /* Time type (in seconds since epoch) */

/* Alignment macros */
#define ALIGN_UP(x, align)  (((x) + (align) - 1) & ~((align) - 1))
#define ALIGN_DOWN(x, align) ((x) & ~((align) - 1))

/* Void pointer size */
typedef void*       ptr_void;    /* Void pointer */

#endif /* _TYPES_H */

