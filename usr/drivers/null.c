#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fs.h>

void null_init(void) {
    if (fs_find_file("dev") == NULL) {
        fs_mkdir("dev");
    }
    fs_chdir("dev");
    if (fs_find_file("null") == NULL) {
        fs_create_with_perms("null", FS_PERM_READ | FS_PERM_WRITE);
    }
    fs_chdir("..");
}
