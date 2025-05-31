#include <sys/fs.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int mv_main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: mv <source> <destination>\n");
        return 1;
    }

    const char *source = argv[1];
    const char *dest = argv[2];

    // Check if source exists
    int source_index = -1;
    for (size_t i = 0; i < root_fs.file_count; i++) {
        if (strcmp(root_fs.files[i].name, source) == 0) {
            source_index = i;
            break;
        }
    }

    if (source_index == -1) {
        printf("Error: Source file '%s' not found\n", source);
        return 2;
    }

    // Check if destination already exists
    for (size_t i = 0; i < root_fs.file_count; i++) {
        if (strcmp(root_fs.files[i].name, dest) == 0) {
            printf("Error: Destination file '%s' already exists\n", dest);
            return 3;
        }
    }

    // Check if source is open
    if (root_fs.files[source_index].is_open) {
        printf("Error: Cannot move open file '%s'\n", source);
        return 4;
    }

    // Check if destination filename is valid
    if (strlen(dest) >= MAX_FILENAME_LEN) {
        printf("Error: Destination filename too long\n");
        return 5;
    }

    // Perform the move by renaming
    strncpy(root_fs.files[source_index].name, dest, MAX_FILENAME_LEN);

    printf("Moved '%s' to '%s'\n", source, dest);
    return 0;
}