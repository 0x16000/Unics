#ifndef _STDINT_H
#define _STDINT_H

/* Define fixed-width integer types */
#ifndef _UINT8_T
#define _UINT8_T
typedef unsigned char uint8_t;    /* 8-bit unsigned integer */
#endif

#ifndef _INT8_T
#define _INT8_T
typedef signed char int8_t;       /* 8-bit signed integer */
#endif

#ifndef _UINT16_T
#define _UINT16_T
typedef unsigned short uint16_t;  /* 16-bit unsigned integer */
#endif

#ifndef _INT16_T
#define _INT16_T
typedef signed short int16_t;     /* 16-bit signed integer */
#endif

#ifndef _UINT32_T
#define _UINT32_T
typedef unsigned int uint32_t;    /* 32-bit unsigned integer */
#endif

#ifndef _INT32_T
#define _INT32_T
typedef signed int int32_t;       /* 32-bit signed integer */
#endif

#ifndef _UINT64_T
#define _UINT64_T
typedef unsigned long uint64_t;   /* 64-bit unsigned integer */
#endif

#ifndef _INT64_T
#define _INT64_T
typedef signed long int64_t;      /* 64-bit signed integer */
#endif

/* Integer types for specific widths */
#ifndef _UINTPTR_T
#define _UINTPTR_T
typedef unsigned long uintptr_t;  /* Unsigned integer for pointer size */
#endif

#ifndef _INTPTR_T
#define _INTPTR_T
typedef signed long intptr_t;     /* Signed integer for pointer size */
#endif

/* Integer types for specific width sizes */
#ifndef _INTMAX_T
#define _INTMAX_T
typedef long long int intmax_t;   /* Maximum width signed integer */
#endif

#ifndef _UINTMAX_T
#define _UINTMAX_T
typedef unsigned long long uintmax_t; /* Maximum width unsigned integer */
#endif

/* Define "fast" integer types */
#ifndef _FAST8_T
#define _FAST8_T
typedef int int_fast8_t;   /* Fast 8-bit signed integer (use int) */
#endif

#ifndef _FAST16_T
#define _FAST16_T
typedef int int_fast16_t;  /* Fast 16-bit signed integer (use int) */
#endif

#ifndef _FAST32_T
#define _FAST32_T
typedef int int_fast32_t;  /* Fast 32-bit signed integer (use int) */
#endif

#ifndef _FAST64_T
#define _FAST64_T
typedef long int int_fast64_t;  /* Fast 64-bit signed integer (use long) */
#endif

#ifndef _UINT_FAST8_T
#define _UINT_FAST8_T
typedef unsigned int uint_fast8_t;  /* Fast 8-bit unsigned integer (use unsigned int) */
#endif

#ifndef _UINT_FAST16_T
#define _UINT_FAST16_T
typedef unsigned int uint_fast16_t;  /* Fast 16-bit unsigned integer (use unsigned int) */
#endif

#ifndef _UINT_FAST32_T
#define _UINT_FAST32_T
typedef unsigned int uint_fast32_t;  /* Fast 32-bit unsigned integer (use unsigned int) */
#endif

#ifndef _UINT_FAST64_T
#define _UINT_FAST64_T
typedef unsigned long int uint_fast64_t;  /* Fast 64-bit unsigned integer (use unsigned long) */
#endif

/* Integer types for minimum width */
#ifndef _INTLEAST8_T
#define _INTLEAST8_T
typedef signed char int_least8_t;   /* Least width signed 8-bit integer */
#endif

#ifndef _UINTLEAST8_T
#define _UINTLEAST8_T
typedef unsigned char uint_least8_t;   /* Least width unsigned 8-bit integer */
#endif

#ifndef _INTLEAST16_T
#define _INTLEAST16_T
typedef signed short int_least16_t;  /* Least width signed 16-bit integer */
#endif

#ifndef _UINTLEAST16_T
#define _UINTLEAST16_T
typedef unsigned short uint_least16_t;  /* Least width unsigned 16-bit integer */
#endif

#ifndef _INTLEAST32_T
#define _INTLEAST32_T
typedef signed int int_least32_t;   /* Least width signed 32-bit integer */
#endif

#ifndef _UINTLEAST32_T
#define _UINTLEAST32_T
typedef unsigned int uint_least32_t;   /* Least width unsigned 32-bit integer */
#endif

#ifndef _INTLEAST64_T
#define _INTLEAST64_T
typedef signed long int_least64_t;  /* Least width signed 64-bit integer */
#endif

#ifndef _UINTLEAST64_T
#define _UINTLEAST64_T
typedef unsigned long uint_least64_t;  /* Least width unsigned 64-bit integer */
#endif

/* Limits */
#ifndef INT8_MAX
#define INT8_MAX  127
#endif
#ifndef INT8_MIN
#define INT8_MIN  (-128)
#endif
#ifndef UINT8_MAX
#define UINT8_MAX  255
#endif

#ifndef INT16_MAX
#define INT16_MAX  32767
#endif
#ifndef INT16_MIN
#define INT16_MIN  (-32768)
#endif
#ifndef UINT16_MAX
#define UINT16_MAX  65535
#endif

#ifndef INT32_MAX
#define INT32_MAX  2147483647
#endif
#ifndef INT32_MIN
#define INT32_MIN  (-2147483648)
#endif
#ifndef UINT32_MAX
#define UINT32_MAX  4294967295U
#endif

#ifndef INT64_MAX
#define INT64_MAX  9223372036854775807LL
#endif
#ifndef INT64_MIN
#define INT64_MIN  (-9223372036854775808LL)
#endif
#ifndef UINT64_MAX
#define UINT64_MAX  18446744073709551615ULL
#endif

/* Size of pointers */
#ifndef PTRDIFF_MAX
#define PTRDIFF_MAX  ((ptrdiff_t)(~((ptrdiff_t)1 << (8 * sizeof(ptrdiff_t) - 1))))
#endif
#ifndef PTRDIFF_MIN
#define PTRDIFF_MIN  ((ptrdiff_t)(1 << (8 * sizeof(ptrdiff_t) - 1)))
#endif

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t)-1)
#endif

#ifndef SSIZE_MAX
#define SSIZE_MAX ((ssize_t)(SIZE_MAX / 2))
#endif

/* Integer constants for easy portability */
#define INT8_C(x)  (x)
#define UINT8_C(x) (x ## U)
#define INT16_C(x) (x)
#define UINT16_C(x) (x ## U)
#define INT32_C(x) (x)
#define UINT32_C(x) (x ## U)
#define INT64_C(x) (x ## L)
#define UINT64_C(x) (x ## UL)

#endif /* _STDINT_H */
