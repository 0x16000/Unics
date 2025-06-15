#ifndef UNISTD_H
#define UNISTD_H

#include <stddef.h>
#include <sys/cdefs.h>
#include <stdint.h>

typedef int64_t off_t;
typedef int pid_t;

__BEGIN_DECLS

/* File operation flags */
#define O_RDONLY  0x01
#define O_WRONLY  0x02
#define O_RDWR    (O_RDONLY | O_WRONLY)
#define O_CREAT   0x04
#define O_TRUNC   0x08

/* Seek constants */
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

/* File descriptor constants */
#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

/* Function prototypes */

int open(const char *pathname, int flags, ...);
int close(int fd);
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
off_t lseek(int fd, off_t offset, int whence);

int unlink(const char *pathname);

int chdir(const char *path);
char *getcwd(char *buf, size_t size);

int isatty(int fd);
int access(const char *pathname, int mode);

unsigned int sleep(unsigned int seconds);

pid_t getpid(void);

__END_DECLS

#endif /* UNISTD_H */
