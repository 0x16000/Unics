#include <sys/fs.h>
#include <stdio.h>

/* Error codes for fs_mkdir */
#define FS_FULL          -1
#define NAME_TOO_LONG    -2
#define ALREADY_EXISTS   -3

/* Application return codes */
#define SUCCESS          0
#define USAGE_ERROR      1
#define FS_ERROR        2
#define NAME_ERROR      3
#define EXISTS_ERROR    4
#define UNKNOWN_ERROR   5

int mkdir_main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: mkdir <dirname>\n");
        return USAGE_ERROR;
    }

    int result = fs_mkdir(argv[1]);
    switch (result) {
        case 0:
            printf("Directory '%s' created successfully.\n", argv[1]);
            return SUCCESS;
        case FS_FULL:
            fprintf(stderr, "Error: Filesystem full.\n");
            return FS_ERROR;
        case NAME_TOO_LONG:
            fprintf(stderr, "Error: Directory name too long.\n");
            return NAME_ERROR;
        case ALREADY_EXISTS:
            fprintf(stderr, "Error: File or directory '%s' already exists.\n", argv[1]);
            return EXISTS_ERROR;
        default:
            fprintf(stderr, "Error: Unknown error creating directory.\n");
            return UNKNOWN_ERROR;
    }
}
