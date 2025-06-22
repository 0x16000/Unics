#include <stdio.h>
#include <sys/fs.h>
#include <vga.h>

int ls_main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    if (root_fs.file_count == 0) {
        return 0;  // Empty directory
    }

    // Save current color settings
    uint8_t original_color = vga_get_color();

    for (size_t i = 0; i < root_fs.file_count; i++) {
        File *f = &root_fs.files[i];
        // Only show files in current directory
        if (f->parent == current_dir) {
            if (f->is_dir) {
                // Set color to light blue for directories
                vga_set_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
            } else {
                // Set color to light grey for files
                vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
            }
            
            printf("%s%s ", f->name, f->is_dir ? "/" : "");
        }
    }

    // Restore original color
    vga_set_color(original_color & 0x0F, (original_color >> 4) & 0x0F);
    
    printf("\n");  // newline after all entries

    return 0;
}
