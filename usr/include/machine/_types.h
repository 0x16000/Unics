#ifndef _MACHINE_TYPES_H
#define _MACHINE_TYPES_H

/* Exact-width integer types */
typedef signed char       __int8_t;
typedef unsigned char     __uint8_t;
typedef signed short      __int16_t;
typedef unsigned short    __uint16_t;
typedef signed int        __int32_t;
typedef unsigned int      __uint32_t;
typedef signed long long  __int64_t;
typedef unsigned long long __uint64_t;

/* Pointer types */
typedef signed long       __intptr_t;
typedef unsigned long     __uintptr_t;

/* Size type (typically matches pointer size) */
typedef unsigned long     __size_t;
typedef signed long       __ssize_t;

/* Minimum-width integer types */
typedef __int8_t          __int_least8_t;
typedef __uint8_t         __uint_least8_t;
typedef __int16_t         __int_least16_t;
typedef __uint16_t        __uint_least16_t;
typedef __int32_t         __int_least32_t;
typedef __uint32_t        __uint_least32_t;
typedef __int64_t         __int_least64_t;
typedef __uint64_t        __uint_least64_t;

/* Fast integer types */
typedef __int32_t         __int_fast8_t;
typedef __uint32_t        __uint_fast8_t;
typedef __int32_t         __int_fast16_t;
typedef __uint32_t        __uint_fast16_t;
typedef __int32_t         __int_fast32_t;
typedef __uint32_t        __uint_fast32_t;
typedef __int64_t         __int_fast64_t;
typedef __uint64_t        __uint_fast64_t;

/* Maximum-width integer types */
typedef __int64_t         __intmax_t;
typedef __uint64_t        __uintmax_t;


#endif /* _MACHINE_TYPES_H */