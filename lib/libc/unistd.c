#include <sys/unistd.h>
#include <stdarg.h>
#include <string.h>
#include <sys/fs.h>
#include <stdio.h>

/* Simple file descriptor table, map fd -> File* */
#define MAX_FDS 32
static File *fd_table[MAX_FDS] = {0};

/* Helper to allocate fd */
static int alloc_fd(File *file) {
    for (int i = 3; i < MAX_FDS; i++) {  // reserve 0,1,2
        if (fd_table[i] == NULL) {
            fd_table[i] = file;
            file->is_open = true;
            file->position = 0;
            return i;
        }
    }
    return -1;  // no fds available
}

/* Helper to get File* from fd */
static File *get_file(int fd) {
    if (fd < 0 || fd >= MAX_FDS) return NULL;
    return fd_table[fd];
}

/* open */
int open(const char *pathname, int flags, ...) {
    (void)flags;  // ignoring flags besides O_CREAT here

    File *file = fs_find_file(pathname);
    if (!file && (flags & O_CREAT)) {
        int res = fs_create(pathname);
        if (res < 0)
            return -1;
        file = fs_find_file(pathname);
    }
    if (!file) return -1;

    int fd = alloc_fd(file);
    if (fd < 0) return -1;

    if (flags & O_TRUNC) {
        if (file->data) {
            memset(file->data, 0, file->allocated_size);
            file->size = 0;
            file->position = 0;
        }
    }

    return fd;
}

/* close */
int close(int fd) {
    File *file = get_file(fd);
    if (!file) return -1;
    file->is_open = false;
    fd_table[fd] = NULL;
    return 0;
}

/* read */
ssize_t read(int fd, void *buf, size_t count) {
    File *file = get_file(fd);
    if (!file || !file->is_open) return -1;
    if (file->position >= file->size) return 0;  // EOF

    size_t bytes_to_read = count;
    if (file->position + bytes_to_read > file->size)
        bytes_to_read = file->size - file->position;

    memcpy(buf, file->data + file->position, bytes_to_read);
    file->position += bytes_to_read;
    return bytes_to_read;
}

/* write */
ssize_t write(int fd, const void *buf, size_t count) {
    File *file = get_file(fd);
    if (!file || !file->is_open) return -1;

    size_t needed_size = file->position + count;
    if (needed_size > file->allocated_size) {
        // Try to resize file buffer
        if (fs_resize_file(file, needed_size) != FS_SUCCESS)
            return -1;
    }

    memcpy(file->data + file->position, buf, count);
    file->position += count;
    if (file->position > file->size)
        file->size = file->position;

    return count;
}

/* lseek */
off_t lseek(int fd, off_t offset, int whence) {
    File *file = get_file(fd);
    if (!file || !file->is_open) return -1;

    off_t new_pos = 0;
    switch (whence) {
        case SEEK_SET: new_pos = offset; break;
        case SEEK_CUR: new_pos = (off_t)file->position + offset; break;
        case SEEK_END: new_pos = (off_t)file->size + offset; break;
        default: return -1;
    }
    if (new_pos < 0 || (size_t)new_pos > file->size) return -1;

    file->position = (size_t)new_pos;
    return new_pos;
}

/* unlink */
int unlink(const char *pathname) {
    return fs_delete(pathname);
}

/* chdir */
int chdir(const char *path) {
    return fs_chdir(path);
}

/* getcwd */
char *getcwd(char *buf, size_t size) {
    return fs_getcwd(buf, size);
}

/* isatty - no terminals, so always false */
int isatty(int fd) {
    (void)fd;
    return 0;
}

/* access - checks existence */
int access(const char *pathname, int mode) {
    (void)mode; // ignoring mode checks for now
    File *file = fs_find_file(pathname);
    return file ? 0 : -1;
}

/* sleep - busy loop stub */
unsigned int sleep(unsigned int seconds) {
    // No real timer, just a busy loop for demo
    for (volatile unsigned int i = 0; i < seconds * 1000000; i++) {}
    return 0;
}

/* getpid - single process system */
pid_t getpid(void) {
    return 1;
}
