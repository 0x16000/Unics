#include <sys/fs.h>
#include <stdio.h>
#include <string.h>

int rm_main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: rm <filename>\n");
        return 1;
    }

    const char *filename = argv[1];
    
    // Try to delete the file
    int result = fs_delete(filename);
    
    switch (result) {
        case 0:
            printf("File '%s' deleted successfully.\n", filename);
            return 0;
        case -1:
            printf("Error: File '%s' is currently open.\n", filename);
            return 2;
        case -2:
            printf("Error: File '%s' not found.\n", filename);
            return 3;
        default:
            printf("Unknown error occurred while deleting '%s'.\n", filename);
            return 4;
    }
}