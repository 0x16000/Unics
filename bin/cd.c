#include <sys/fs.h>
#include <stdio.h>
#include <string.h>

#define PATH_MAX 256

extern File *current_dir;
char cwd[PATH_MAX] = "/";

int cd_main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: cd <directory>\n");
        return 1;
    }

    const char *target = argv[1];

    if (strcmp(target, "/") == 0) {
        current_dir = NULL;
        strncpy(cwd, "/", PATH_MAX);
        return 0;
    }

    if (strcmp(target, "..") == 0) {
        if (current_dir && current_dir->parent) {
            current_dir = current_dir->parent;
        } else {
            current_dir = NULL; // already at root
        }

        // Update cwd string
        if (!current_dir) {
            strncpy(cwd, "/", PATH_MAX);
        } else {
            snprintf(cwd, PATH_MAX, "/%s", current_dir->name);
        }
        return 0;
    }

    // Search for directory under current_dir (or root)
    for (size_t i = 0; i < root_fs.file_count; i++) {
        File *f = &root_fs.files[i];

        if (f->is_dir && strcmp(f->name, target) == 0 &&
            ((current_dir == NULL && f->parent == NULL) || (f->parent == current_dir))) {
            current_dir = f;
            snprintf(cwd, PATH_MAX, "/%s", current_dir->name);
            return 0;
        }
    }

    printf("cd: no such directory: %s\n", target);
    return 2;
}
