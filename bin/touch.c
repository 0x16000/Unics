#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fs.h>

int touch_main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: touch <filename> [filename2 ...]\n");
        return 1;
    }

    int success_count = 0;

    for (int i = 1; i < argc; i++) {
        const char *filename = argv[i];

        // Check if file already exists
        int exists = 0;
        for (size_t j = 0; j < root_fs.file_count; j++) {
            if (strcmp(root_fs.files[j].name, filename) == 0) {
                exists = 1;
                break;
            }
        }

        if (exists) {
            printf("'%s' already exists\n", filename);
            continue;
        }

        // Create new file
        int result = fs_create(filename); // no fs passed
        if (result == 0) {
            printf("Created '%s'\n", filename);
            success_count++;
        } else {
            printf("Error creating '%s' (code %d)\n", filename, result);
        }
    }

    if (success_count > 0) {
        printf("Successfully created %d file(s)\n", success_count);
    } else {
        printf("No files were created\n");
    }

    return (success_count > 0) ? 0 : 2;
}