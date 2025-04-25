#ifndef _UNISTD_H
#define _UNISTD_H

#include <stdint.h>  // Include for intptr_t, uintptr_t, etc.

intptr_t syscall0(int num);
intptr_t syscall1(int num, intptr_t arg1);
intptr_t syscall2(int num, intptr_t arg1, intptr_t arg2);
intptr_t syscall3(int num, intptr_t arg1, intptr_t arg2, intptr_t arg3);
unsigned int sleep(unsigned int seconds);

void _exit(int status) __attribute__((noreturn));

#endif /* _UNISTD_H */
