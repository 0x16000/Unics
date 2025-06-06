#include <sys/fs.h>
#include <stdio.h>
#include <string.h>

int rm_main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: rm <filename> [filename...]\n");
        return 1;
    }

    int exit_code = 0;

    for (int i = 1; i < argc; ++i) {
        const char *filename = argv[i];
        int result = fs_delete(filename);

        switch (result) {
            case 0:
                printf("File '%s' deleted successfully.\n", filename);
                break;
            case -1:
                printf("Error: File '%s' is currently open.\n", filename);
                if (exit_code < 2) exit_code = 2;
                break;
            case -2:
                printf("Error: File '%s' not found.\n", filename);
                if (exit_code < 3) exit_code = 3;
                break;
            default:
                printf("Unknown error occurred while deleting '%s'.\n", filename);
                if (exit_code < 4) exit_code = 4;
                break;
        }
    }

    return exit_code;
}
