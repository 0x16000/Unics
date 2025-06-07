#include <sys/fs.h>
#include <stdio.h>
#include <string.h>

extern File *current_dir;
char cwd[PATH_MAX] = "/";

static void update_cwd() {
    if (!current_dir) {
        strcpy(cwd, "/");
        return;
    }

    // Build path by traversing up to root
    char temp[PATH_MAX] = {0};
    File *dir = current_dir;
    
    while (dir && dir->parent) {
        char path_part[PATH_MAX] = {0};
        snprintf(path_part, sizeof(path_part), "/%s", dir->name);
        memmove(temp + strlen(path_part), temp, strlen(temp) + 1);
        memcpy(temp, path_part, strlen(path_part));
        dir = dir->parent;
    }

    // Handle root case
    if (strlen(temp) == 0) {
        strcpy(temp, "/");
    }

    strncpy(cwd, temp, PATH_MAX);
}

int cd_main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: cd <directory>\n");
        return 1;
    }

    const char *target = argv[1];

    if (strcmp(target, "/") == 0) {
        current_dir = NULL;
        update_cwd();
        return 0;
    }

    if (strcmp(target, "..") == 0) {
        if (current_dir && current_dir->parent) {
            current_dir = current_dir->parent;
        } else {
            current_dir = NULL; // already at root
        }
        update_cwd();
        return 0;
    }

    // Search for directory under current_dir (or root)
    for (size_t i = 0; i < root_fs.file_count; i++) {
        File *f = &root_fs.files[i];
        if (f->is_dir && strcmp(f->name, target) == 0 &&
            ((current_dir == NULL && f->parent == NULL) || (f->parent == current_dir))) {
            current_dir = f;
            update_cwd();
            return 0;
        }
    }

    printf("cd: no such directory: %s\n", target);
    return 2;
}
