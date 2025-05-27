#ifndef _STDDEF_H
#define _STDDEF_H 1

/* Prevent C++ name mangling */
#ifdef __cplusplus
extern "C" {
#endif

/* GCC specific attributes and optimizations */
#ifdef __GNUC__
    #define PACKED        __attribute__((packed))
    #define ALIGNED(x)    __attribute__((aligned(x)))
    #define UNUSED        __attribute__((unused))
    #define DEPRECATED    __attribute__((deprecated))
    #define PURE         __attribute__((pure))
    #define CONST        __attribute__((const))
    #define NORETURN     __attribute__((noreturn))
#else
    #define PACKED
    #define ALIGNED(x)
    #define UNUSED
    #define DEPRECATED
    #define PURE
    #define CONST
    #define NORETURN
#endif

/* NULL pointer definition */
#ifdef __cplusplus
    #define NULL 0L
#else
    #define NULL ((void*)0)
#endif

/* Size type definition */
#ifndef _SIZE_T_DEFINED
    #define _SIZE_T_DEFINED
    typedef unsigned int size_t;
#endif

/* Signed size type (for differences) */
#ifndef _SSIZE_T_DEFINED
    #define _SSIZE_T_DEFINED
    typedef int ssize_t;
#endif

/* Pointer difference type */
#ifndef _PTRDIFF_T_DEFINED
    #define _PTRDIFF_T_DEFINED
    typedef int ptrdiff_t;
#endif

/* Maximum alignment type */
typedef struct {
    long long _ll ALIGNED(8);
    long double _ld ALIGNED(8);
    void* _ptr ALIGNED(4);
} max_align_t;

/* Wide character type */
#ifndef _WCHAR_T_DEFINED
    #define _WCHAR_T_DEFINED
    typedef unsigned short wchar_t;
#endif

/* Verify fundamental type sizes */
_Static_assert(sizeof(size_t) == 4, "size_t must be 4 bytes");
_Static_assert(sizeof(ptrdiff_t) == 4, "ptrdiff_t must be 4 bytes");
_Static_assert(sizeof(wchar_t) == 2, "wchar_t must be 2 bytes");

/* Offset of a member within a struct */
#define offsetof(type, member) __builtin_offsetof(type, member)

/* Container of macro - get struct pointer from member pointer */
#define container_of(ptr, type, member) ({ \
    const typeof(((type *)0)->member) * __mptr = (ptr); \
    (type *)((char *)__mptr - offsetof(type, member)); })

/* Alignment utilities */
#define ALIGN_MASK(x, mask)    (((x) + (mask)) & ~(mask))
#define ALIGN_UP(x, align)     ALIGN_MASK(x, (typeof(x))(align) - 1)
#define ALIGN_DOWN(x, align)   ((x) & ~((typeof(x))(align) - 1))
#define IS_ALIGNED(x, align)   (((x) & ((typeof(x))(align) - 1)) == 0)

/* Size calculation utilities */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define FIELD_SIZEOF(type, field) sizeof(((type *)0)->field)

/* Memory barrier macros */
#define MEMORY_BARRIER()           __asm__ __volatile__("" ::: "memory")
#define READ_BARRIER()            __asm__ __volatile__("" ::: "memory")
#define WRITE_BARRIER()           __asm__ __volatile__("" ::: "memory")

/* Debug utilities */
#ifdef DEBUG
    #define ASSERT_ALIGNMENT(ptr, align) \
        do { \
            if (!IS_ALIGNED((uintptr_t)(ptr), align)) { \
                __builtin_trap(); \
            } \
        } while(0)

    #define VERIFY_PTR(ptr) \
        do { \
            if ((ptr) == NULL) { \
                __builtin_trap(); \
            } \
        } while(0)
#else
    #define ASSERT_ALIGNMENT(ptr, align) ((void)0)
    #define VERIFY_PTR(ptr) ((void)0)
#endif

/* Compile-time type checking */
#define MUST_BE_ARRAY(arr) \
    _Static_assert(!__builtin_types_compatible_p(typeof(arr), typeof(&(arr)[0])), \
                  "Not an array")

/* Build-time calculations */
#define COMPILETIME_ASSERT(cond, msg) \
    _Static_assert(cond, msg)

/* Function type safety */
#define RETURNS_NONNULL     __attribute__((returns_nonnull))
#define NONNULL_PARAMS(...) __attribute__((nonnull(__VA_ARGS__)))

/* Architecture-specific optimizations */
#define CACHELINE_SIZE 64
#define PAGE_SIZE 4096

#define CACHELINE_ALIGNED ALIGNED(CACHELINE_SIZE)
#define PAGE_ALIGNED     ALIGNED(PAGE_SIZE)

/* Version information */
#define STDDEF_VERSION "1.0.0"
#define STDDEF_VERSION_MAJOR 1
#define STDDEF_VERSION_MINOR 0
#define STDDEF_VERSION_PATCH 0

#ifdef __cplusplus
}
#endif

#endif /* _STDDEF_H */
