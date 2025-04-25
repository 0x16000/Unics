#include <sys/bootscreen.h>
#include <vga.h>
#include <string.h>

/* ASCII art logo for BSD-inspired OS */
static const char *logo[] = {
    "______  ___________ ",
    "| ___ \\/  ___|  _  \\",
    "| |_/ /\\ `--.| | | |",
    "| ___ \\ `--. \\ | | |",
    "| |_/ /\\__/ / |/ /",
    "\\____/ \\____/|___/",  
};


static const char *version_str = "MyOS 0.1 [BSD Inspired]";

void bootscreen_init(void) {
    vga_initialize();
    vga_disable_cursor();
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    bootscreen_clear();
    bootscreen_draw_logo();
}

void bootscreen_clear(void) {
    vga_clear();
}

void bootscreen_draw_logo(void) {
    int logo_lines = sizeof(logo) / sizeof(logo[0]);
    int start_y = 2;
    
    /* Draw logo */
    for (int i = 0; i < logo_lines; i++) {
        int len = strlen(logo[i]);
        int x = (VGA_WIDTH - len) / 2;
        vga_puts_at(logo[i], x, start_y + i);
    }
    
    /* Draw version string */
    bootscreen_write_centered(start_y + logo_lines + 1, version_str);
}

void bootscreen_write_centered(int y, const char *str) {
    int len = strlen(str);
    int x = (VGA_WIDTH - len) / 2;
    vga_puts_at(str, x, y);
}

void bootscreen_write_status(const char *status) {
    static const int status_line = VGA_HEIGHT - 4;
    
    /* Clear status line */
    for (int x = 0; x < VGA_WIDTH; x++) {
        vga_putchar_at(' ', x, status_line);
    }
    
    /* Write new status */
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_puts_at(status, 2, status_line);
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}

void bootscreen_progress(int percentage) {
    static const int progress_y = VGA_HEIGHT - 2;
    const int bar_width = VGA_WIDTH - 4;
    const int filled = (percentage * bar_width) / 100;
    
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    
    /* Draw progress bar */
    for (int x = 2; x < VGA_WIDTH - 2; x++) {
        char c = (x - 2) < filled ? '=' : ' ';
        vga_putchar_at(c, x, progress_y);
    }
    
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}

void bootscreen_error(const char *message) {
    const int error_y = VGA_HEIGHT - 6;
    int len = strlen(message);
    int x = (VGA_WIDTH - len) / 2;
    
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_RED);
    vga_puts_at(message, x, error_y);
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}
