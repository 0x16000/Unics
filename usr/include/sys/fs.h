#ifndef FS_H
#define FS_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_FILES 64
#define MAX_FILENAME_LEN 32
#define MAX_FILE_SIZE 8192
#define PATH_MAX 256
#define FS_BLOCK_SIZE 512

// Error codes
#define FS_SUCCESS 0
#define FS_ERR_FULL -1
#define FS_ERR_NAME_TOO_LONG -2
#define FS_ERR_EXISTS -3
#define FS_ERR_NOT_FOUND -4
#define FS_ERR_NO_MEMORY -5
#define FS_ERR_FILE_OPEN -6
#define FS_ERR_NOT_OPEN -7
#define FS_ERR_NOT_EMPTY -8
#define FS_ERR_INVALID_PATH -9
#define FS_ERR_SEEK_BOUNDS -10

// File permissions
#define FS_PERM_READ    0x01
#define FS_PERM_WRITE   0x02
#define FS_PERM_EXEC    0x04
#define FS_PERM_DEFAULT (FS_PERM_READ | FS_PERM_WRITE)

typedef struct File {
    char name[MAX_FILENAME_LEN];
    uint8_t *data;
    size_t size;
    size_t allocated_size;
    bool is_open;
    size_t position;
    bool is_dir;
    uint8_t permissions;
    uint32_t inode;
    struct File *parent;
    uint64_t created_time;
    uint64_t modified_time;
} File;

typedef struct {
    File files[MAX_FILES];
    size_t file_count;
    uint32_t next_inode;
    bool free_list[MAX_FILES];  // Track free slots
} FileSystem;

// Global filesystem instance
extern FileSystem root_fs;
extern File *current_dir;

// Core filesystem functions
void fs_init(void);
int fs_create(const char *filename);
int fs_create_with_perms(const char *filename, uint8_t permissions);
int fs_delete(const char *filename);
int fs_open(FileSystem *fs, const char *filename);
int fs_close(FileSystem *fs, const char *filename);

// Directory operations
int fs_mkdir(const char *dirname);
int fs_chdir(const char *dirname);
char* fs_getcwd(char *buffer, size_t size);
void fs_list(FileSystem *fs);

// File I/O operations
size_t fs_read(FileSystem *fs, const char *filename, void *buffer, size_t size);
size_t fs_write(FileSystem *fs, const char *filename, const void *buffer, size_t size);
int fs_seek(FileSystem *fs, const char *filename, size_t offset);
size_t fs_tell(FileSystem *fs, const char *filename);

// Utility functions
File* fs_find_file(const char *filename);
File* fs_find_file_in_dir(const char *filename, File *dir);
int fs_validate_filename(const char *filename);
void fs_print_stats(void);
int fs_resize_file(File *file, size_t new_size);

// Path handling
int fs_resolve_path(const char *path, File **parent, char *basename);
bool fs_is_valid_path_char(char c);

#endif // FS_H
