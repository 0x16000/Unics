#include <sys/fs.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MV_SUCCESS 0
#define MV_ERR_ARGS 1
#define MV_ERR_SOURCE_NOT_FOUND 2
#define MV_ERR_DEST_EXISTS 3
#define MV_ERR_SOURCE_OPEN 4
#define MV_ERR_NAME_TOO_LONG 5

int mv_main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: mv <source> <destination>\n");
        return MV_ERR_ARGS;
    }

    const char *source = argv[1];
    const char *dest = argv[2];

    // Check if destination filename is valid first (fail fast)
    if (strlen(dest) >= MAX_FILENAME_LEN) {
        printf("Error: Destination filename too long\n");
        return MV_ERR_NAME_TOO_LONG;
    }

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
        return MV_ERR_SOURCE_NOT_FOUND;
    }

    // Check if destination already exists
    for (size_t i = 0; i < root_fs.file_count; i++) {
        if (strcmp(root_fs.files[i].name, dest) == 0) {
            printf("Error: Destination file '%s' already exists\n", dest);
            return MV_ERR_DEST_EXISTS;
        }
    }

    // Check if source is open
    if (root_fs.files[source_index].is_open) {
        printf("Error: Cannot move open file '%s'\n", source);
        return MV_ERR_SOURCE_OPEN;
    }

    // Perform the move by renaming
    strncpy(root_fs.files[source_index].name, dest, MAX_FILENAME_LEN - 1);
    root_fs.files[source_index].name[MAX_FILENAME_LEN - 1] = '\0';

    printf("Moved '%s' to '%s'\n", source, dest);
    return MV_SUCCESS;
}
