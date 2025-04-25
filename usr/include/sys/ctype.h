#ifndef _CTYPE_H
#define _CTYPE_H 1

#include <sys/cdefs.h>

__BEGIN_DECLS

/* Character classification functions */
int isalnum(int c);
int isalpha(int c);
int isblank(int c);
int iscntrl(int c);
int isdigit(int c);
int isgraph(int c);
int islower(int c);
int isprint(int c);
int ispunct(int c);
int isspace(int c);
int isupper(int c);
int isxdigit(int c);

/* Character conversion functions */
int tolower(int c);
int toupper(int c);

/* POSIX extensions */
int isascii(int c);
int toascii(int c);

__END_DECLS

#endif /* _CTYPE_H */
