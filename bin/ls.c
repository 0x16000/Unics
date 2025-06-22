#include <stdio.h>
#include <sys/fs.h>

int ls_main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    if (root_fs.file_count == 0) {
        return 0;  // Empty directory
    }

    for (size_t i = 0; i < root_fs.file_count; i++) {
        File *f = &root_fs.files[i];
        // Only show files in current directory
        if (f->parent == current_dir) {
            printf("%s%s ", f->name, f->is_dir ? "/" : "");
        }
    }

    printf("\n");  // newline after all entries

    return 0;
}
