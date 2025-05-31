#ifndef _STDIO_H
#define _STDIO_H 1

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

/* Standard definitions */
#define EOF (-1)
#define stdin  0
#define stdout 1
#define stderr 2

/* Standard I/O functions */
int printf(const char *format, ...);
int scanf(const char *format, ...);
int getchar(void);
int putchar(int c);
int ungetc(int c, int fd);

/* Rest of your existing stdio.h content */
int puts(const char *str);
int fprintf(int fd, const char *format, ...);
int sprintf(char *buf, const char *format, ...);
int snprintf(char *buf, size_t n, const char *format, ...);
int fputs(const char *str, int fd);
int sscanf(const char *str, const char *format, ...);

#endif /* _STDIO_H */
