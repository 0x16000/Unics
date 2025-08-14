#include <sys/fs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/_null.h>

// Global filesystem instance
FileSystem root_fs;
File *current_dir = NULL;

static uint64_t fs_get_time(void) {
    return (uint64_t)time(NULL);
}

static size_t fs_find_free_slot(void) {
    for (size_t i = 0; i < MAX_FILES; i++) {
        if (root_fs.free_list[i]) {
            return i;
        }
    }
    return MAX_FILES;
}

static inline bool fs_has_perm(File *file, uint8_t perm) {
    return (file->permissions & perm) == perm;
}

void fs_init(void) {
    memset(&root_fs, 0, sizeof(FileSystem));
    root_fs.file_count = 0;
    root_fs.next_inode = 1;
    
    // Initialize free list
    for (size_t i = 0; i < MAX_FILES; i++) {
        root_fs.free_list[i] = true;
    }

    // Create root directory
    File *root_dir = &root_fs.files[0];
    root_fs.free_list[0] = false;
    
    strcpy(root_dir->name, "/");
    root_dir->is_dir = true;
    root_dir->parent = NULL;
    root_dir->data = NULL;
    root_dir->size = 0;
    root_dir->allocated_size = 0;
    root_dir->is_open = false;
    root_dir->position = 0;
    root_dir->permissions = FS_PERM_READ | FS_PERM_WRITE | FS_PERM_EXEC;
    root_dir->inode = root_fs.next_inode++;
    root_dir->created_time = fs_get_time();
    root_dir->modified_time = root_dir->created_time;

    root_fs.file_count = 1;
    current_dir = root_dir;
}

int fs_validate_filename(const char *filename) {
    if (!filename || strlen(filename) == 0) {
        return FS_ERR_INVALID_PATH;
    }
    
    if (strlen(filename) >= MAX_FILENAME_LEN) {
        return FS_ERR_NAME_TOO_LONG;
    }
    
    // Check for invalid characters
    for (const char *p = filename; *p; p++) {
        if (!fs_is_valid_path_char(*p)) {
            return FS_ERR_INVALID_PATH;
        }
    }
    
    // Reject . and .. as regular filenames (they're special)
    if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0) {
        return FS_ERR_INVALID_PATH;
    }
    
    return FS_SUCCESS;
}

bool fs_is_valid_path_char(char c) {
    // Allow alphanumeric, dash, underscore, and dot
    return (c >= 'a' && c <= 'z') || 
           (c >= 'A' && c <= 'Z') || 
           (c >= '0' && c <= '9') || 
           (c == '-' || c == '_') || c == '.';
}

File* fs_find_file_in_dir(const char *filename, File *dir) {
    if (!filename || !dir) return NULL;
    
    for (size_t i = 0; i < root_fs.file_count; i++) {
        if (!root_fs.free_list[i] && 
            root_fs.files[i].parent == dir && 
            strcmp(root_fs.files[i].name, filename) == 0) {
            return &root_fs.files[i];
        }
    }
    return NULL;
}

File* fs_find_file(const char *filename) {
    return fs_find_file_in_dir(filename, current_dir);
}

int fs_resize_file(File *file, size_t new_size) {
    if (!file || file->is_dir) return FS_ERR_INVALID_PATH;
    
    if (new_size > MAX_FILE_SIZE) {
        new_size = MAX_FILE_SIZE;
    }
    
    if (new_size > file->allocated_size) {
        // Need to grow the file
        size_t new_allocated = ((new_size + FS_BLOCK_SIZE - 1) / FS_BLOCK_SIZE) * FS_BLOCK_SIZE;
        uint8_t *new_data = realloc(file->data, new_allocated);
        if (!new_data) {
            return FS_ERR_NO_MEMORY;
        }
        file->data = new_data;
        file->allocated_size = new_allocated;
    }
    
    file->size = new_size;
    file->modified_time = fs_get_time();
    return FS_SUCCESS;
}

int fs_create_with_perms(const char *filename, uint8_t permissions) {
    int res = fs_validate_filename(filename);
    if (res != FS_SUCCESS) return res;

    if (fs_find_file(filename)) return FS_ERR_EXISTS;

    size_t slot = fs_find_free_slot();
    if (slot == MAX_FILES) return FS_ERR_FULL;

    File *file = &root_fs.files[slot];
    root_fs.free_list[slot] = false;

    strncpy(file->name, filename, MAX_FILENAME_LEN - 1);
    file->name[MAX_FILENAME_LEN - 1] = '\0';

    file->allocated_size = FS_BLOCK_SIZE;
    file->data = malloc(file->allocated_size);
    if (!file->data) {
        root_fs.free_list[slot] = true;
        return FS_ERR_NO_MEMORY;
    }

    file->size = 0;
    file->is_open = false;
    file->is_dir = false;
    file->parent = current_dir;
    file->position = 0;
    file->permissions = permissions;
    file->inode = root_fs.next_inode++;
    file->created_time = fs_get_time();
    file->modified_time = file->created_time;

    root_fs.file_count++;
    return FS_SUCCESS;
}

int fs_create(const char *filename) {
    return fs_create_with_perms(filename, FS_PERM_DEFAULT);
}

int fs_delete(const char *filename) {
    File *file = fs_find_file(filename);
    if (!file) {
        return FS_ERR_NOT_FOUND;
    }

    if (file->is_open) {
        return FS_ERR_FILE_OPEN;
    }

    // Check if it's root directory
    if (file == &root_fs.files[0]) {
        return FS_ERR_INVALID_PATH;
    }

    // Handle directories
    if (file->is_dir) {
        // Check if directory is empty
        for (size_t i = 0; i < MAX_FILES; i++) {
            if (!root_fs.free_list[i] && root_fs.files[i].parent == file) {
                return FS_ERR_NOT_EMPTY;
            }
        }
        
        // If we're deleting current directory, move to parent
        if (current_dir == file) {
            current_dir = file->parent;
        }
    } else {
        // Free file data
        if (file->data) {
            free(file->data);
            file->data = NULL;
        }
    }

    // Mark slot as free
    size_t file_index = file - root_fs.files;
    root_fs.free_list[file_index] = true;
    memset(file, 0, sizeof(File));
    root_fs.file_count--;

    return FS_SUCCESS;
}

int fs_open(FileSystem *fs, const char *filename) {
    (void)fs; // Unused parameter for API compatibility
    File *file = fs_find_file(filename);
    if (!file) {
        return FS_ERR_NOT_FOUND;
    }

    if (file->is_dir) {
        return FS_ERR_INVALID_PATH;
    }
    
    if (file->is_open) {
        return FS_ERR_FILE_OPEN;
    }

    file->is_open = true;
    file->position = 0;
    return FS_SUCCESS;
}

int fs_close(FileSystem *fs, const char *filename) {
    (void)fs; // Unused parameter for API compatibility
    File *file = fs_find_file(filename);
    if (!file) {
        return FS_ERR_NOT_FOUND;
    }

    if (!file->is_open) {
        return FS_ERR_NOT_OPEN;
    }

    file->is_open = false;
    return FS_SUCCESS;
}

size_t fs_read(FileSystem *fs, const char *filename, void *buffer, size_t size) {
    (void)fs; // Unused parameter for API compatibility
    if (!buffer || size == 0) return 0;
    
    File *file = fs_find_file(filename);
    if (!file || !file->is_open || file->is_dir) {
        return 0;
    }
    
    if (!(file->permissions & FS_PERM_READ)) {
        return 0;
    }
    
    size_t bytes_available = file->size > file->position ? 
                            file->size - file->position : 0;
    size_t bytes_to_read = size < bytes_available ? size : bytes_available;
    
    if (bytes_to_read > 0) {
        memcpy(buffer, file->data + file->position, bytes_to_read);
        file->position += bytes_to_read;
    }
    
    return bytes_to_read;
}

size_t fs_write(FileSystem *fs, const char *filename, const void *buffer, size_t size) {
    (void)fs;
    if (!buffer || size == 0) return 0;

    File *file = fs_find_file(filename);
    if (!file || !file->is_open || file->is_dir) return 0;
    if (!fs_has_perm(file, FS_PERM_WRITE)) return 0;

    size_t required_size = file->position + size;
    if (required_size > file->size) {
        int res = fs_resize_file(file, required_size);
        if (res != FS_SUCCESS) {
            size_t available = file->allocated_size > file->position ? file->allocated_size - file->position : 0;
            if (available == 0) return 0;
            size = size < available ? size : available;
        }
    }

    memcpy(file->data + file->position, buffer, size);
    file->position += size;

    if (file->position > file->size) {
        file->size = file->position;
    }

    file->modified_time = fs_get_time();
    return size;
}

int fs_seek(FileSystem *fs, const char *filename, size_t offset) {
    (void)fs; // Unused parameter for API compatibility
    File *file = fs_find_file(filename);
    if (!file) {
        return FS_ERR_NOT_FOUND;
    }
    
    if (!file->is_open) {
        return FS_ERR_NOT_OPEN;
    }
    
    if (offset > file->size) {
        return FS_ERR_SEEK_BOUNDS;
    }
    
    file->position = offset;
    return FS_SUCCESS;
}

size_t fs_tell(FileSystem *fs, const char *filename) {
    (void)fs; // Unused parameter for API compatibility
    File *file = fs_find_file(filename);
    if (!file || !file->is_open) {
        return 0;
    }
    return file->position;
}

int fs_mkdir(const char *dirname) {
    int validation_result = fs_validate_filename(dirname);
    if (validation_result != FS_SUCCESS) {
        return validation_result;
    }

    // Check if directory already exists
    if (fs_find_file(dirname) != NULL) {
        return FS_ERR_EXISTS;
    }

    // Find free slot
    size_t free_slot = MAX_FILES;
    for (size_t i = 0; i < MAX_FILES; i++) {
        if (root_fs.free_list[i]) {
            free_slot = i;
            break;
        }
    }
    
    if (free_slot == MAX_FILES) {
        return FS_ERR_FULL;
    }

    // Create new directory
    File *new_dir = &root_fs.files[free_slot];
    root_fs.free_list[free_slot] = false;
    
    strncpy(new_dir->name, dirname, MAX_FILENAME_LEN - 1);
    new_dir->name[MAX_FILENAME_LEN - 1] = '\0';
    new_dir->is_dir = true;
    new_dir->data = NULL;
    new_dir->size = 0;
    new_dir->allocated_size = 0;
    new_dir->is_open = false;
    new_dir->position = 0;
    new_dir->parent = current_dir;
    new_dir->permissions = FS_PERM_READ | FS_PERM_WRITE | FS_PERM_EXEC;
    new_dir->inode = root_fs.next_inode++;
    new_dir->created_time = fs_get_time();
    new_dir->modified_time = new_dir->created_time;

    root_fs.file_count++;
    return FS_SUCCESS;
}

int fs_chdir(const char *dirname) {
    if (strcmp(dirname, "..") == 0) {
        if (current_dir->parent != NULL) {
            current_dir = current_dir->parent;
            return FS_SUCCESS;
        }
        return FS_ERR_INVALID_PATH; // Already at root
    }

    File *target_dir = fs_find_file(dirname);
    if (!target_dir) {
        return FS_ERR_NOT_FOUND;
    }
    
    if (!target_dir->is_dir) {
        return FS_ERR_INVALID_PATH;
    }

    current_dir = target_dir;
    return FS_SUCCESS;
}

char* fs_getcwd(char *buffer, size_t size) {
    if (!buffer || size == 0) return NULL;

    File *dirs[MAX_FILES];
    size_t depth = 0;

    File *dir = current_dir;
    while (dir) {
        dirs[depth++] = dir;
        dir = dir->parent;
        if (depth == MAX_FILES) break; // prevent infinite loop
    }

    // Build path in reverse order
    size_t pos = 0;
    for (size_t i = depth; i > 0; i--) {
        File *d = dirs[i - 1];
        if (d->parent == NULL) {
            // root directory
            if (pos == 0) buffer[pos++] = '/';
            break;
        }
        size_t len = strlen(d->name);
        if (pos + 1 + len >= size) return NULL;
        buffer[pos++] = '/';
        memcpy(buffer + pos, d->name, len);
        pos += len;
    }
    buffer[pos] = '\0';
    return buffer;
}

void fs_list(FileSystem *fs) {
    (void)fs; // Unused parameter for API compatibility
    printf("Directory listing for: ");
    char cwd[PATH_MAX];
    if (fs_getcwd(cwd, sizeof(cwd))) {
        printf("%s\n", cwd);
    } else {
        printf("(unknown)\n");
    }
    
    printf("%-20s %-8s %-8s %-10s %s\n", "Name", "Type", "Size", "Perms", "Modified");
    printf("%-20s %-8s %-8s %-10s %s\n", "----", "----", "----", "-----", "--------");
    
    for (size_t i = 0; i < MAX_FILES; i++) {
        if (root_fs.free_list[i]) continue;
        
        File *f = &root_fs.files[i];
        if (f->parent == current_dir) {
            const char *type = f->is_dir ? "DIR" : "FILE";
            char perms[4] = "---";
            if (f->permissions & FS_PERM_READ) perms[0] = 'r';
            if (f->permissions & FS_PERM_WRITE) perms[1] = 'w';
            if (f->permissions & FS_PERM_EXEC) perms[2] = 'x';
            
            printf("%-20s %-8s %-8zu %-10s %lu\n", 
                   f->name, type, f->size, perms, (unsigned long)f->modified_time);
        }
    }
}

void fs_print_stats(void) {
    size_t used_files = 0;
    size_t total_data_size = 0;
    size_t total_allocated = 0;
    
    for (size_t i = 0; i < MAX_FILES; i++) {
        if (!root_fs.free_list[i]) {
            used_files++;
            File *f = &root_fs.files[i];
            total_data_size += f->size;
            total_allocated += f->allocated_size;
        }
    }
    
    printf("Filesystem Statistics:\n");
    printf("  Files/Directories: %zu / %d\n", used_files, MAX_FILES);
    printf("  Data Size: %zu bytes\n", total_data_size);
    printf("  Allocated: %zu bytes\n", total_allocated);
    printf("  Next Inode: %u\n", root_fs.next_inode);
}
