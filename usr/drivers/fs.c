#include <sys/fs.h>
#include <stdlib.h>
#include <string.h>

// The global filesystem instance
FileSystem root_fs;

void fs_init(void) {
    memset(&root_fs, 0, sizeof(FileSystem));
    root_fs.file_count = 0;
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
    new_file->position = 0;
    root_fs.file_count++;

    return 0; // Success
}

int fs_delete(const char *filename) {
    for (size_t i = 0; i < root_fs.file_count; i++) {
        if (strcmp(root_fs.files[i].name, filename) == 0) {
            if (root_fs.files[i].is_open) return -1;
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
        // Simple output - in a real OS you'd use a proper print function
        const char *status = fs->files[i].is_open ? "open" : "closed";
        // This assumes you have some way to output strings
        // In a real OS, you'd replace this with your kernel's print function
        // For now, we'll just leave it as a placeholder
        // printf("[%s] %s (%zu bytes)\n", status, fs->files[i].name, fs->files[i].size);
    }
}