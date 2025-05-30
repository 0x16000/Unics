#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/fs.h>

#define BUFFER_INCREMENT 1024

static char *buffer = NULL;
static size_t buf_size = 0;
static size_t content_len = 0;
static bool insert_mode = false;

// Reads a line from console input into buf, maxlen including null terminator.
// Returns number of characters read (excluding null) on success,
// -1 on EOF or error.
int read_line(char *buf, int max_len) {
    int i = 0;
    int c;

    while (i < max_len - 1) {
        c = getchar();   // read one character
        if (c == EOF) {
            if (i == 0) return -1;
            break;
        }
        if (c == '\n') {
            putchar('\n');
            break;
        }
        putchar(c);  // echo character immediately
        buf[i++] = (char)c;
    }
    buf[i] = '\0';
    return i;
}

void print_help(void) {
    printf("Simple ed commands:\n");
    printf("  w         : start inserting text\n");
    printf("  .         : end inserting text (only in insert mode)\n");
    printf("  q         : quit editor\n");
    printf("  h         : show this help\n");
    printf("  :w filename : write buffer to file\n");
}

void grow_buffer(size_t min_size) {
    if (buf_size >= min_size) return;
    size_t new_size = buf_size ? buf_size : BUFFER_INCREMENT;
    while (new_size < min_size) {
        new_size += BUFFER_INCREMENT;
    }
    char *new_buffer = realloc(buffer, new_size);
    if (!new_buffer) {
        printf("Out of memory\n");
        exit(1);
    }
    buffer = new_buffer;
    buf_size = new_size;
}

void insert_text(const char *text) {
    size_t len = strlen(text);
    grow_buffer(content_len + len + 2);  // +1 for '\n' +1 for '\0'
    memcpy(buffer + content_len, text, len);
    content_len += len;
    buffer[content_len++] = '\n';  // add newline after each inserted line
    buffer[content_len] = '\0';
}

int write_to_file(const char *filename) {
    int rc = fs_create(filename);
    if (rc != 0 && rc != -3) { 
        // -3 = file already exists, which is fine
        printf("Error creating file: %d\n", rc);
        return -1;
    }

    rc = fs_open(&root_fs, filename);
    if (rc != 0) {
        printf("Error opening file: %d\n", rc);
        return -1;
    }

    size_t written = fs_write(&root_fs, filename, buffer, content_len);
    if (written != content_len) {
        printf("Error writing to file: wrote %zu of %zu bytes\n", written, content_len);
        fs_close(&root_fs, filename);
        return -1;
    }

    fs_close(&root_fs, filename);
    printf("Wrote %zu bytes to %s\n", content_len, filename);
    return 0;
}

int ed_main(void) {
    char line[512];
    print_help();
    grow_buffer(BUFFER_INCREMENT);
    buffer[0] = '\0';

    while (true) {
        if (insert_mode) {
            int n = read_line(line, sizeof(line));
            if (n == -1) break; // EOF

            // If line is "." alone, end insert mode
            if (strcmp(line, ".") == 0) {
                insert_mode = false;
                printf("End insert mode.\n");
            } else {
                insert_text(line);
            }
        } else {
            printf("ed> ");
            int n = read_line(line, sizeof(line));
            if (n == -1) break; // EOF

            // Trim trailing newline if any (shouldn't be present from read_line)
            // but keep just in case
            size_t len = strlen(line);
            if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';

            if (strcmp(line, "q") == 0) {
                break;
            } else if (strcmp(line, "w") == 0) {
                insert_mode = true;
                printf("Insert mode. Type lines, end with a '.' on a line by itself.\n");
            } else if (strncmp(line, ":w ", 3) == 0) {
                const char *filename = line + 3;
                if (write_to_file(filename) != 0) {
                    printf("Failed to write to %s\n", filename);
                }
            } else if (strcmp(line, "h") == 0) {
                print_help();
            } else {
                printf("Unknown command '%s'. Type 'h' for help.\n", line);
            }
        }
    }

    free(buffer);
    printf("Bye.\n");
    return 0;
}
