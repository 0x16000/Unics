#include <sys/fs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// The global filesystem instance
FileSystem root_fs;
File *current_dir = NULL;

void fs_init(void) {
    memset(&root_fs, 0, sizeof(FileSystem));
    root_fs.file_count = 0;

    // Create root directory
    File *root_dir = &root_fs.files[root_fs.file_count++];
    strcpy(root_dir->name, "/");
    root_dir->is_dir = true;
    root_dir->parent = NULL;
    root_dir->data = NULL;
    root_dir->size = 0;
    root_dir->is_open = false;
    root_dir->position = 0;

    current_dir = root_dir;
}

int fs_create(const char *filename) {
        if (root_fs.file_count >= MAX_FILES) {
        return -1;
    }

    if (strlen(filename) >= MAX_FILENAME_LEN) {
        return -2; // Filename too long
    }

    // Check if file already exists
    for (size_t i = 0; i < root_fs.file_count; i++) {
        if (strcmp(root_fs.files[i].name, filename) == 0) {
            return -3; // File already exists
        }
    }

    // Create new file
    File *new_file = &root_fs.files[root_fs.file_count];
    strncpy(new_file->name, filename, MAX_FILENAME_LEN);
    new_file->data = malloc(MAX_FILE_SIZE);
    if (!new_file->data) {
        return -4; // Memory allocation failed
    }

    new_file->size = 0;
    new_file->is_open = false;
    new_file->is_dir = false;
    new_file->parent = current_dir;
    new_file->position = 0;
    root_fs.file_count++;

    return 0; // Success
}

int fs_delete(const char *filename) {
    for (size_t i = 0; i < root_fs.file_count; i++) {
        if (strcmp(root_fs.files[i].name, filename) == 0) {
            if (root_fs.files[i].is_open) return -1;
            if (root_fs.files[i].is_dir) return -3; // Can't delete directory

            free(root_fs.files[i].data);
            for (size_t j = i; j < root_fs.file_count - 1; j++) {
                root_fs.files[j] = root_fs.files[j + 1];
            }
            root_fs.file_count--;
            return 0;
        }
    }
    return -2;
}

int fs_open(FileSystem *fs, const char *filename) {
    for (size_t i = 0; i < fs->file_count; i++) {
        if (strcmp(fs->files[i].name, filename) == 0) {
            if (fs->files[i].is_open) {
                return -1; // File already open
            }
            
            fs->files[i].is_open = true;
            fs->files[i].position = 0;
            return 0; // Success
        }
    }
    
    return -2; // File not found
}

int fs_close(FileSystem *fs, const char *filename) {
    for (size_t i = 0; i < fs->file_count; i++) {
        if (strcmp(fs->files[i].name, filename) == 0) {
            if (!fs->files[i].is_open) {
                return -1; // File not open
            }
            
            fs->files[i].is_open = false;
            return 0; // Success
        }
    }
    
    return -2; // File not found
}

size_t fs_read(FileSystem *fs, const char *filename, void *buffer, size_t size) {
    for (size_t i = 0; i < fs->file_count; i++) {
        if (strcmp(fs->files[i].name, filename) == 0) {
            if (!fs->files[i].is_open) {
                return 0; // File not open
            }
            
            size_t bytes_available = fs->files[i].size - fs->files[i].position;
            size_t bytes_to_read = size < bytes_available ? size : bytes_available;
            
            memcpy(buffer, fs->files[i].data + fs->files[i].position, bytes_to_read);
            fs->files[i].position += bytes_to_read;
            
            return bytes_to_read;
        }
    }
    
    return 0; // File not found
}

size_t fs_write(FileSystem *fs, const char *filename, const void *buffer, size_t size) {
    for (size_t i = 0; i < fs->file_count; i++) {
        if (strcmp(fs->files[i].name, filename) == 0) {
            if (!fs->files[i].is_open) {
                return 0; // File not open
            }
            
            size_t space_available = MAX_FILE_SIZE - fs->files[i].position;
            size_t bytes_to_write = size < space_available ? size : space_available;
            
            memcpy(fs->files[i].data + fs->files[i].position, buffer, bytes_to_write);
            fs->files[i].position += bytes_to_write;
            
            // Update file size if we wrote past the previous end
            if (fs->files[i].position > fs->files[i].size) {
                fs->files[i].size = fs->files[i].position;
            }
            
            return bytes_to_write;
        }
    }
    
    return 0; // File not found
}

int fs_seek(FileSystem *fs, const char *filename, size_t offset) {
    for (size_t i = 0; i < fs->file_count; i++) {
        if (strcmp(fs->files[i].name, filename) == 0) {
            if (!fs->files[i].is_open) {
                return -1; // File not open
            }
            
            if (offset > fs->files[i].size) {
                return -2; // Offset beyond file size
            }
            
            fs->files[i].position = offset;
            return 0; // Success
        }
    }
    
    return -3; // File not found
}

void fs_list(FileSystem *fs) {
    for (size_t i = 0; i < fs->file_count; i++) {
        File *f = &fs->files[i];
        if (f->parent == current_dir) {  // Only list files/dirs in current directory
            const char *type = f->is_dir ? "DIR " : "FILE";
            const char *status = f->is_open ? "open" : "closed";
            printf("[%s] %s (%s) - %zu bytes\n", status, f->name, type, f->size);
        }
    }
}

int fs_mkdir(const char *dirname) {
    if (root_fs.file_count >= MAX_FILES) {
        return -1; // Too many files/directories
    }

    if (strlen(dirname) >= MAX_FILENAME_LEN) {
        return -2; // Name too long
    }

    // Check if directory already exists
    for (size_t i = 0; i < root_fs.file_count; i++) {
        if (strcmp(root_fs.files[i].name, dirname) == 0) {
            return -3; // Already exists
        }
    }

    // Create new directory
    File *new_file = &root_fs.files[root_fs.file_count];
    strncpy(new_file->name, dirname, MAX_FILENAME_LEN);
    new_file->is_dir = true;
    new_file->data = NULL; // Directories don't have data
    new_file->size = 0;
    new_file->is_open = false;
    new_file->position = 0;
    new_file->parent = current_dir;

    root_fs.file_count++;
    return 0; // Success
}

int fs_chdir(const char *dirname) {
    if (strcmp(dirname, "..") == 0) {
        if (current_dir->parent != NULL) {
            current_dir = current_dir->parent;
            return 0; // success
        }
        // Already at root
        return -2;
    }

    for (size_t i = 0; i < root_fs.file_count; i++) {
        File *f = &root_fs.files[i];
        if (f->parent == current_dir && strcmp(f->name, dirname) == 0 && f->is_dir) {
            current_dir = f;
            return 0;
        }
    }

    return -1; // Directory not found
}
