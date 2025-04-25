#ifndef _STDARG_H
#define _STDARG_H

/* Ensure the system stdio.h does not interfere */
#undef va_list
#define va_list char*

/* Define va_start, va_arg, va_end */
#define va_start(vl, last) (vl = (va_list)&last + sizeof(last))
#define va_arg(vl, type)   (*(type*) (vl += sizeof(type) - sizeof(char*)))
#define va_end(vl)         (void)(vl)
#define va_copy(dest, src) (dest = src)

#endif /* _STDARG_H */
