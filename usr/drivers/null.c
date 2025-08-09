#include <sys/fs.h>

void null_init(void) {
    // Check if "dev" directory exists, create if missing
    if (fs_find_file("dev") == NULL) {
        if (fs_mkdir("dev") != 0) {
            // Failed to create "dev" directory, handle error or abort
            return;
        }
    }

    // Change directory to "dev"
    if (fs_chdir("dev") != 0) {
        // Failed to change directory to "dev"
        return;
    }

    // Check if "null" device file exists, create if missing
    if (fs_find_file("null") == NULL) {
        int ret = fs_create_with_perms("null", FS_PERM_READ | FS_PERM_WRITE);
        if (ret != 0) {
            fs_chdir("..");
            return;
        }
    }

    // Return to previous directory, check for error but ignore for now
    fs_chdir("..");
}
