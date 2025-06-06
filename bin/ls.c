#include <stdio.h>
#include <sys/fs.h>

int ls_main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    if (root_fs.file_count == 0) {
        printf("");
        return 0;
    }

    for (size_t i = 0; i < root_fs.file_count; i++) {
        File *f = &root_fs.files[i];
        if (f->parent == current_dir) {
            printf("%s%s\n", f->name, f->is_dir ? "/" : "");
        }
    }

    return 0;
}
