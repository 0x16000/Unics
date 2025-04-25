#include <vga.h>

int clear_main(int argc, char **argv) {
    (void) argc; // Indicate that argc is unused
    (void) argv; // Indicate that argv is unused
    vga_clear();
    return 0;
}
