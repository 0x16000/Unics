#include <sys/fs.h>  // only include what is needed

void null_init(void) {
    // Check if "dev" directory exists, create if missing
    if (fs_find_file("dev") == NULL) {
        if (fs_mkdir("dev") != 0) {
            // handle error if needed
            return;
        }
    }

    // Change directory to "dev"
    if (fs_chdir("dev") != 0) {
        // handle error if needed
        return;
    }

    // Check if "null" device file exists, create if missing
    if (fs_find_file("null") == NULL) {
        fs_create_with_perms("null", FS_PERM_READ | FS_PERM_WRITE);
    }

    // Return to previous directory
    fs_chdir("..");
}
