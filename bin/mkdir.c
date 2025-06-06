#include <sys/fs.h>
#include <stdio.h>

int mkdir_main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: mkdir <dirname>\n");
        return 1;
    }

    int result = fs_mkdir(argv[1]);
    switch (result) {
        case 0:
            printf("Directory '%s' created.\n", argv[1]);
            return 0;
        case -1:
            printf("Error: Filesystem full.\n");
            return 2;
        case -2:
            printf("Error: Directory name too long.\n");
            return 3;
        case -3:
            printf("Error: File or directory already exists.\n");
            return 4;
        default:
            printf("Unknown error.\n");
            return 5;
    }
}
