#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

#define SYS_EXIT     1
#define SYS_READ     3
#define SYS_WRITE    4
#define SYS_OPEN     5
#define SYS_CLOSE    6
#define SYS_LSEEK    19
#define SYS_GETPID   20

int32_t syscall(int32_t number,
                int32_t arg1,
                int32_t arg2,
                int32_t arg3,
                int32_t arg4,
                int32_t arg5);

int32_t sys_exit(int status);
int32_t sys_read(int fd, void *buf, uint32_t count);
int32_t sys_write(int fd, const void *buf, uint32_t count);
int32_t sys_open(const char *pathname, int flags, int mode);
int32_t sys_close(int fd);
int32_t sys_lseek(int fd, int32_t offset, int whence);
int32_t sys_getpid(void);

#endif // SYSCALL_H
