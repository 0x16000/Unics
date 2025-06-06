#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/fs.h>

#define BUFFER_SIZE 512

int cp_main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: cp <source> <destination>\n");
        return EXIT_FAILURE;
    }

    const char *src = argv[1];
    const char *dst = argv[2];

    // Allocate buffer
    uint8_t buffer[BUFFER_SIZE];

    // Open source file
    int src_idx = fs_open(&root_fs, src);
    if (src_idx < 0) {
        fprintf(stderr, "cp: cannot open source file '%s'\n", src);
        return EXIT_FAILURE;
    }

    // Create destination file
    if (fs_create(dst) < 0) {
        fprintf(stderr, "cp: cannot create destination file '%s'\n", dst);
        fs_close(&root_fs, src);
        return EXIT_FAILURE;
    }

    // Open destination file
    int dst_idx = fs_open(&root_fs, dst);
    if (dst_idx < 0) {
        fprintf(stderr, "cp: cannot open destination file '%s'\n", dst);
        fs_close(&root_fs, src);
        return EXIT_FAILURE;
    }

    // Copy loop
    while (1) {
        size_t bytes_read = fs_read(&root_fs, src, buffer, BUFFER_SIZE);
        if (bytes_read == 0) {
            break; // EOF
        }
        size_t bytes_written = fs_write(&root_fs, dst, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            fprintf(stderr, "cp: write error to '%s'\n", dst);
            fs_close(&root_fs, src);
            fs_close(&root_fs, dst);
            return EXIT_FAILURE;
        }
    }

    fs_close(&root_fs, src);
    fs_close(&root_fs, dst);

    return EXIT_SUCCESS;
}
