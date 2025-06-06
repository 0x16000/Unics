#ifndef FS_H
#define FS_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_FILES 32
#define MAX_FILENAME_LEN 32
#define MAX_FILE_SIZE 4096

typedef struct File {
    char name[MAX_FILENAME_LEN];
    uint8_t *data;
    size_t size;
    bool is_open;
    size_t position;
    bool is_dir;
    struct File *parent;
} File;

typedef struct {
    File files[MAX_FILES];
    size_t file_count;
} FileSystem;

// Declare the global filesystem instance
extern FileSystem root_fs;
extern File *current_dir;

// Filesystem functions
void fs_init(void);
int fs_create(const char *filename);
int fs_delete(const char *filename);
int fs_open(FileSystem *fs, const char *filename);
int fs_close(FileSystem *fs, const char *filename);
int fs_mkdir(const char *dirname);
int fs_chdir(const char *dirname);

// File I/O operations
size_t fs_read(FileSystem *fs, const char *filename, void *buffer, size_t size);
size_t fs_write(FileSystem *fs, const char *filename, const void *buffer, size_t size);
int fs_seek(FileSystem *fs, const char *filename, size_t offset);

// Directory operations
void fs_list(FileSystem *fs);

#endif // FS_H
