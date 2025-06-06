#include <stdio.h>
#include <sys/fs.h>

int pwd_main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    if (current_dir == NULL) {
        printf("/\n");
        return 0;
    }

    // Collect directories from current up to root
    const size_t max_depth = 64;
    const File *stack[max_depth];
    size_t depth = 0;

    const File *dir = current_dir;
    while (dir != NULL && depth < max_depth) {
        stack[depth++] = dir;
        dir = dir->parent;
    }

    // Print from root down to current_dir
    for (ssize_t i = depth - 1; i >= 0; i--) {
        // Skip empty names to avoid extra slashes
        if (stack[i]->name[0] == '\0') {
            continue;
        }
        printf("%s", stack[i]->name);
        if (i > 0) {
            printf("");
        }
    }

    printf("\n");
    return 0;
}
