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
    size_t bytes_read;

    while ((bytes_read = fs_read(&root_fs, filename, buffer, BUFFER_SIZE)) > 0) {
        for (size_t i = 0; i < bytes_read; i++) {
            putchar(buffer[i]);  // Use your libc putchar()
        }
    }

    fs_close(&root_fs, filename);

    return 0;
}
