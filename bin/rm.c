#include <sys/fs.h>
#include <stdio.h>
#include <string.h>

#define EXIT_SUCCESS 0
#define EXIT_GENERAL_ERROR 1
#define EXIT_FILE_IN_USE 2
#define EXIT_NOT_FOUND 3

int rm_main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: rm <filename> [filename...]\n");
        return EXIT_GENERAL_ERROR;
    }

    int exit_code = EXIT_SUCCESS;
    int verbose = 0;

    // Check for -v flag (simple implementation)
    if (argc > 1 && strcmp(argv[1], "-v") == 0) {
        verbose = 1;
        argc--;
        argv++;
    }

    for (int i = 1; i < argc; ++i) {
        const char *filename = argv[i];
        int result = fs_delete(filename);

        switch (result) {
            case 0:
                if (verbose) {
                    printf("removed '%s'\n", filename);
                }
                break;
            case -1:
                fprintf(stderr, "rm: cannot remove '%s': File is in use\n", filename);
                exit_code = EXIT_FILE_IN_USE;
                break;
            case -2:
                fprintf(stderr, "rm: cannot remove '%s': No such file\n", filename);
                if (exit_code == EXIT_SUCCESS) {
                    exit_code = EXIT_NOT_FOUND;
                }
                break;
            default:
                fprintf(stderr, "rm: unknown error deleting '%s'\n", filename);
                exit_code = EXIT_GENERAL_ERROR;
                break;
        }
    }

    return exit_code;
}
