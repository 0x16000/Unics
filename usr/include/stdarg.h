#ifndef _STDARG_H
#define _STDARG_H 1

/* Prevent C++ name mangling */
#ifdef __cplusplus
extern "C" {
#endif

/* GCC specific attributes */
#ifdef __GNUC__
    #define VA_UNUSED       __attribute__((unused))
    #define VA_FORMAT(m,n)  __attribute__((format(printf, m, n)))
    #define VA_NONNULL(...) __attribute__((nonnull(__VA_ARGS__)))
#else
    #define VA_UNUSED
    #define VA_FORMAT(m,n)
    #define VA_NONNULL(...)
#endif

/* Type definition for va_list */
typedef __builtin_va_list va_list;

/* Core variable argument macros */
#define va_start(v,l)  __builtin_va_start(v,l)
#define va_end(v)      __builtin_va_end(v)
#define va_arg(v,l)    __builtin_va_arg(v,l)
#define va_copy(d,s)   __builtin_va_copy(d,s)

/* Extended safety macros for debug builds */
#ifdef DEBUG
    /* Verify argument count matches format string */
    #define VA_VERIFY_COUNT(fmt, count) \
        do { \
            const char* _fmt = (fmt); \
            int _expected = 0; \
            while (*_fmt) { \
                if (*_fmt++ == '%' && *_fmt != '%') { \
                    _expected++; \
                } \
            } \
            if (_expected != (count)) { \
                __builtin_trap(); \
            } \
        } while(0)

    /* Verify format string is not NULL */
    #define VA_VERIFY_FORMAT(fmt) \
        do { \
            if ((fmt) == NULL) { \
                __builtin_trap(); \
            } \
        } while(0)

    /* Track va_list state */
    #define VA_START(ap, last) \
        do { \
            va_start(ap, last); \
            _va_started = 1; \
        } while(0)

    #define VA_END(ap) \
        do { \
            va_end(ap); \
            _va_started = 0; \
        } while(0)

    static volatile int _va_started = 0;

    /* Verify va_list state */
    #define VA_VERIFY_STATE() \
        do { \
            if (_va_started != 1) { \
                __builtin_trap(); \
            } \
        } while(0)
#else
    #define VA_VERIFY_COUNT(fmt, count) ((void)0)
    #define VA_VERIFY_FORMAT(fmt) ((void)0)
    #define VA_START(ap, last) va_start(ap, last)
    #define VA_END(ap) va_end(ap)
    #define VA_VERIFY_STATE() ((void)0)
#endif

/* Safe variable argument wrapper structure */
typedef struct {
    va_list ap;
    int started;
    const char* func_name;
} va_list_safe;

/* Safe variable argument handling functions */
static inline void VA_UNUSED va_start_safe(va_list_safe* safe_list, 
                                         const char* func_name, 
                                         const void* last) {
    safe_list->started = 1;
    safe_list->func_name = func_name;
    va_start(safe_list->ap, last);
}

static inline void VA_UNUSED va_end_safe(va_list_safe* safe_list) {
    if (safe_list->started) {
        va_end(safe_list->ap);
        safe_list->started = 0;
    }
}

/* Macro to automatically get function name */
#define VA_START_SAFE(safe_list, last) \
    va_start_safe(safe_list, __func__, last)

/* Type-safe argument access templates */
#define va_arg_type(ap, type) \
    __builtin_choose_expr(__builtin_types_compatible_p(type, int), \
        va_arg(ap, int), \
    __builtin_choose_expr(__builtin_types_compatible_p(type, double), \
        va_arg(ap, double), \
    __builtin_choose_expr(__builtin_types_compatible_p(type, void*), \
        va_arg(ap, void*), \
        va_arg(ap, type))))

/* Variable argument function declaration helper */
#define VARARGS_FUNC(ret_type, name, first_arg, ...) \
    ret_type name(first_arg, ...) VA_FORMAT(1, 2)

/* Version information */
#define STDARG_VERSION "1.0.0"
#define STDARG_VERSION_MAJOR 1
#define STDARG_VERSION_MINOR 0
#define STDARG_VERSION_PATCH 0

#ifdef __cplusplus
}
#endif

#endif /* _STDARG_H */
