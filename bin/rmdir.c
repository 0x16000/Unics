#include <stdio.h>
#include <sys/fs.h>
#include <string.h>
#include <stdlib.h>

int rmdir_main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: rmdir <directory>\n");
        return 1;
    }

    const char *dirname = argv[1];

    // Search for the directory in root_fs matching current_dir as parent
    for (size_t i = 0; i < root_fs.file_count; i++) {
        File *f = &root_fs.files[i];
        if (f->parent == current_dir && strcmp(f->name, dirname) == 0) {
            if (!f->is_dir) {
                printf("rmdir: '%s' is not a directory\n", dirname);
                return 2;
            }

            // Check if directory is empty (no files with this directory as parent)
            bool is_empty = true;
            for (size_t j = 0; j < root_fs.file_count; j++) {
                if (root_fs.files[j].parent == f) {
                    is_empty = false;
                    break;
                }
            }

            if (!is_empty) {
                printf("rmdir: '%s' is not empty\n", dirname);
                return 3;
            }

            // Directory is empty - delete it
            if (f->is_open) {
                printf("rmdir: '%s' is currently open\n", dirname);
                return 4;
            }

            free(f->data);

            // Shift remaining files left to fill the gap
            for (size_t k = i; k < root_fs.file_count - 1; k++) {
                root_fs.files[k] = root_fs.files[k + 1];
            }
            root_fs.file_count--;

            printf("Directory '%s' removed successfully.\n", dirname);
            return 0;
        }
    }

    printf("rmdir: directory '%s' not found\n", dirname);
    return 5;
}
