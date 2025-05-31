#include <stdio.h>
#include <sys/fs.h>

int ls_main(int argc, char *argv[]) {
    if (root_fs.file_count == 0) {
        printf("");
        return 0;
    }

    for (size_t i = 0; i < root_fs.file_count; i++) {
        printf("%s \n", root_fs.files[i].name);
    }

    return 0;
}
