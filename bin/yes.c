#include <stdio.h>
#include <keyboard.h>

int yes_main(int argc, char **argv) {
    printf("Press ESC to stop.\n");

    while (1) {
        if (argc > 1) {
            for (int i = 1; i < argc; i++) {
                printf("%s ", argv[i]);
            }
            printf("\n");
        } else {
            printf("y\n");
        }

        if (kb_check_escape()) {
            kb_getchar();
            break;
        }
    }

    printf("\nExiting yes.\n");
    return 0;
}
