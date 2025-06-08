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

    // Open the file (no file handle returned in your API)
    int rc = fs_open(&root_fs, filename);
    if (rc != 0) {
        printf("Error opening file %s (code: %d)\n", filename, rc);
        return 1;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    int retval = 0;

    // Read the file in chunks
    while ((bytes_read = fs_read(&root_fs, filename, buffer, BUFFER_SIZE)) > 0) {
        // Output the read data
        for (ssize_t i = 0; i < bytes_read; i++) {
            putchar(buffer[i]);
        }
    }

    if (bytes_read < 0) {
        printf("Error reading file %s (code: %zd)\n", filename, bytes_read);
        retval = 1;
    }

    // Close the file
    rc = fs_close(&root_fs, filename);
    if (rc != 0) {
        printf("Error closing file %s (code: %d)\n", filename, rc);
        retval = 1;
    }

    return retval;
}
