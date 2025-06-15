#include <sys/syscall.h>

// Using int 0x80 syscall interface for i386
int32_t syscall(int32_t number,
                int32_t arg1,
                int32_t arg2,
                int32_t arg3,
                int32_t arg4,
                int32_t arg5)
{
    int32_t ret;
    asm volatile (
        "int $0x80"
        : "=a"(ret)
        : "a"(number),
          "b"(arg1),
          "c"(arg2),
          "d"(arg3),
          "S"(arg4),
          "D"(arg5)
        : "memory"
    );
    return ret;
}

// syscall wrappers
int32_t sys_exit(int status) {
    return syscall(SYS_EXIT, status, 0, 0, 0, 0);
}

int32_t sys_read(int fd, void *buf, uint32_t count) {
    return syscall(SYS_READ, fd, (int32_t)buf, count, 0, 0);
}

int32_t sys_write(int fd, const void *buf, uint32_t count) {
    return syscall(SYS_WRITE, fd, (int32_t)buf, count, 0, 0);
}

int32_t sys_open(const char *pathname, int flags, int mode) {
    return syscall(SYS_OPEN, (int32_t)pathname, flags, mode, 0, 0);
}

int32_t sys_close(int fd) {
    return syscall(SYS_CLOSE, fd, 0, 0, 0, 0);
}

int32_t sys_lseek(int fd, int32_t offset, int whence) {
    return syscall(SYS_LSEEK, fd, offset, whence, 0, 0);
}

int32_t sys_getpid(void) {
    return syscall(SYS_GETPID, 0, 0, 0, 0, 0);
}

