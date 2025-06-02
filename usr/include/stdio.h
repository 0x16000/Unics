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

#define BUFSIZ 512
#define FILENAME_MAX 256

/* File operations (basic) */
typedef struct {
    int fd;
    int flags;
    int error;
    int eof;
} FILE;

extern FILE *stdin_file;
extern FILE *stdout_file;
extern FILE *stderr_file;

/* Standard I/O functions */
int printf(const char *format, ...);
int scanf(const char *format, ...);
int getchar(void);
int putchar(int c);
int ungetc(int c, int fd);
int puts(const char *str);
int fprintf(int fd, const char *format, ...);
int sprintf(char *buf, const char *format, ...);
int snprintf(char *buf, size_t n, const char *format, ...);
int fputs(const char *str, int fd);
int sscanf(const char *str, const char *format, ...);

/* Additional string functions */
int vprintf(const char *format, va_list ap);
int vsprintf(char *buf, const char *format, va_list ap);
int vsnprintf(char *buf, size_t n, const char *format, va_list ap);

/* Character I/O */
int fgetc(FILE *stream);
int fputc(int c, FILE *stream);
char *gets(char *s);  /* Note: unsafe, consider fgets instead */
char *fgets(char *s, int size, FILE *stream);

/* Error handling */
void perror(const char *s);
int ferror(FILE *stream);
int feof(FILE *stream);
void clearerr(FILE *stream);

/* File positioning (if you add file support later) */
long ftell(FILE *stream);
int fseek(FILE *stream, long offset, int whence);
void rewind(FILE *stream);

/* Buffer control */
int setvbuf(FILE *stream, char *buf, int mode, size_t size);
void setbuf(FILE *stream, char *buf);

#define _IOFBF 0  /* full buffering */
#define _IOLBF 1  /* line buffering */
#define _IONBF 2  /* no buffering */

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#endif /* _STDIO_H */