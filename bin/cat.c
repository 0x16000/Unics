#include <stdio.h>
#include <stdlib.h>
#include <sys/fs.h>

#define BUFFER_SIZE 1024

int cat_main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: cat <filename>\n");
        return 1;
    }

    const char *filename = argv[1];

    int rc = fs_open(&root_fs, filename);
    if (rc != 0) {
        printf("Error opening file: %s\n", filename);
        return 1;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = fs_read(&root_fs, filename, buffer, BUFFER_SIZE)) > 0) {
        for (ssize_t i = 0; i < bytes_read; i++) {
            putchar(buffer[i]);
        }
    }

    if (bytes_read < 0) {
        printf("Error reading file: %s\n", filename);
    }

    rc = fs_close(&root_fs, filename);
    if (rc != 0) {
        printf("Error closing file: %s\n", filename);
    }

    return bytes_read < 0 ? 1 : 0;
}
