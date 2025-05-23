#ifndef VGA_H
#define VGA_H

#include <stdint.h>
#include <stddef.h>

// VGA color enumeration
typedef enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_YELLOW = 14,
    VGA_COLOR_WHITE = 15,
} vga_color_t;

// Create a VGA color byte
static inline uint8_t vga_entry_color(vga_color_t fg, vga_color_t bg) {
    return (uint8_t)(fg | (bg << 4));
}

// Create a VGA entry (character + color)
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t)uc | ((uint16_t)color << 8);
}

// Use a constant expression for default color
#define VGA_COLOR_DEFAULT ((uint8_t)((VGA_COLOR_LIGHT_GREY) | ((VGA_COLOR_BLACK) << 4)))
#define VGA_COLOR_ERROR   ((uint8_t)((VGA_COLOR_WHITE) | ((VGA_COLOR_RED) << 4)))

// Screen dimensions
#define VGA_WIDTH  80
#define VGA_HEIGHT 25

// Tab width
#define VGA_TAB_WIDTH 4

// Function declarations
int vga_initialize(void);
void vga_clear(void);
void vga_putchar(char c);
void vga_puts(const char* str);
void vga_enable_cursor(void);
void vga_disable_cursor(void);
void vga_update_cursor(int x, int y);
void vga_set_color(vga_color_t fg, vga_color_t bg);
void vga_move_cursor(int x, int y);
void vga_swap_buffers(void);
void vga_set_buffer(uint16_t* buffer);
void vga_putdec(uint32_t value, uint8_t digits);
void vga_init_double_buffer(void);
void vga_get_cursor(int *x, int *y);
uint8_t vga_get_color(void);
void vga_puthex(uint32_t num);
void vga_putchar_at(char c, int x, int y);
void vga_puts_at(const char *str, int x, int y);

#endif // VGA_H
