#include <vga.h>
#include <io.h>
#include <string.h>
#include <stdarg.h>

// VGA memory address
#define VGA_MEMORY_ADDR ((uint16_t*) 0xB8000)
static uint16_t* const VGA_MEMORY = VGA_MEMORY_ADDR;

// Cursor control ports
#define VGA_CRTC_ADDR 0x3D4
#define VGA_CRTC_DATA 0x3D5

// Cursor control registers
#define VGA_CURSOR_START_REG 0x0A
#define VGA_CURSOR_END_REG   0x0B
#define VGA_CURSOR_HIGH_REG  0x0E
#define VGA_CURSOR_LOW_REG   0x0F

// Cursor disable value
#define VGA_CURSOR_DISABLE 0x20

// State
static size_t vga_row = 0;
static size_t vga_column = 0;
static uint8_t vga_color = VGA_COLOR_DEFAULT;
static uint16_t* vga_buffer = NULL;
static bool vga_initialized = false;

// Double buffer (optional)
static uint16_t vga_double_buffer[VGA_HEIGHT * VGA_WIDTH];

// Bounds checking helper
static inline bool vga_bounds_check(int x, int y) {
    return (x >= 0 && x < VGA_WIDTH && y >= 0 && y < VGA_HEIGHT);
}

// Helper: scroll the screen up by specified lines
static void vga_scroll_internal(int lines) {
    if (!vga_buffer || lines <= 0) return;
    
    if (lines >= VGA_HEIGHT) {
        vga_clear();
        return;
    }
    
    // Move lines up
    size_t move_size = sizeof(uint16_t) * VGA_WIDTH * (VGA_HEIGHT - lines);
    memmove(vga_buffer, vga_buffer + (VGA_WIDTH * lines), move_size);
    
    // Clear the bottom lines
    uint16_t blank = vga_entry(' ', vga_color);
    for (int line = VGA_HEIGHT - lines; line < VGA_HEIGHT; line++) {
        for (int col = 0; col < VGA_WIDTH; col++) {
            vga_buffer[line * VGA_WIDTH + col] = blank;
        }
    }
    
    // Adjust cursor position
    if (vga_row >= (size_t)lines) {
        vga_row -= lines;
    } else {
        vga_row = 0;
        vga_column = 0;
    }
}

// Initialize VGA
int vga_initialize(void) {
    vga_color = VGA_COLOR_DEFAULT;
    vga_buffer = VGA_MEMORY;
    vga_row = 0;
    vga_column = 0;
    vga_initialized = true;
    vga_clear();
    vga_enable_cursor();
    return VGA_SUCCESS;
}

// Shutdown VGA driver
void vga_shutdown(void) {
    vga_disable_cursor();
    vga_initialized = false;
}

// Check if VGA is initialized
bool vga_is_initialized(void) {
    return vga_initialized;
}

// Clear the screen
void vga_clear(void) {
    if (!vga_buffer) return;
    uint16_t blank = vga_entry(' ', vga_color);
    for (size_t i = 0; i < VGA_HEIGHT * VGA_WIDTH; i++)
        vga_buffer[i] = blank;
    vga_row = 0;
    vga_column = 0;
    vga_update_cursor(0, 0);
}

// Clear a specific area
void vga_clear_area(int x, int y, int width, int height) {
    if (!vga_buffer) return;
    
    uint16_t blank = vga_entry(' ', vga_color);
    for (int row = y; row < y + height && row < VGA_HEIGHT; row++) {
        for (int col = x; col < x + width && col < VGA_WIDTH; col++) {
            if (vga_bounds_check(col, row)) {
                vga_buffer[row * VGA_WIDTH + col] = blank;
            }
        }
    }
}

// Print a character with improved error handling
void vga_putchar(char c) {
    if (!vga_buffer) return;

    switch (c) {
    case '\n':
        vga_column = 0;
        if (++vga_row >= VGA_HEIGHT)
            vga_scroll_internal(1);
        break;
        
    case '\r':
        vga_column = 0;
        break;
        
    case '\b':
        if (vga_column > 0) {
            vga_column--;
        } else if (vga_row > 0) {
            vga_row--;
            vga_column = VGA_WIDTH - 1;
        }
        vga_buffer[vga_row * VGA_WIDTH + vga_column] = vga_entry(' ', vga_color);
        break;
        
    case '\t': {
        size_t next_tab = (vga_column + VGA_TAB_WIDTH) & ~(VGA_TAB_WIDTH - 1);
        while (vga_column < next_tab && vga_column < VGA_WIDTH) {
            vga_buffer[vga_row * VGA_WIDTH + vga_column] = vga_entry(' ', vga_color);
            vga_column++;
        }
        if (vga_column >= VGA_WIDTH) {
            vga_column = 0;
            if (++vga_row >= VGA_HEIGHT)
                vga_scroll_internal(1);
        }
        break;
    }
    
    default:
        if (c >= ' ' && c <= '~') {
            vga_buffer[vga_row * VGA_WIDTH + vga_column] = vga_entry(c, vga_color);
            if (++vga_column >= VGA_WIDTH) {
                vga_column = 0;
                if (++vga_row >= VGA_HEIGHT)
                    vga_scroll_internal(1);
            }
        }
        break;
    }
    vga_update_cursor((int)vga_column, (int)vga_row);
}

// Print a decimal number with N digits (pads with zeros)
void vga_putdec(uint32_t value, uint8_t digits) {
    char buffer[12];
    int i = 0;
    
    if (value == 0) {
        buffer[i++] = '0';
    } else {
        while (value && i < (int)sizeof(buffer) - 1) {
            buffer[i++] = '0' + (value % 10);
            value /= 10;
        }
    }

    while (i < digits && i < (int)sizeof(buffer) - 1)
        buffer[i++] = '0';

    // Output in reverse
    while (i--)
        vga_putchar(buffer[i]);
}

// Print a string
void vga_puts(const char* str) {
    if (!str) return;
    while (*str)
        vga_putchar(*str++);
}

// Simple printf implementation
int vga_printf(const char* format, ...) {
    if (!format) return VGA_ERROR_NULL_POINTER;
    
    va_list args;
    va_start(args, format);
    
    int count = 0;
    while (*format) {
        if (*format == '%' && *(format + 1)) {
            format++; // Skip '%'
            switch (*format) {
                case 'd': {
                    int val = va_arg(args, int);
                    if (val < 0) {
                        vga_putchar('-');
                        val = -val;
                        count++;
                    }
                    vga_putdec((uint32_t)val, 1);
                    count++;
                    break;
                }
                case 'x': {
                    uint32_t val = va_arg(args, uint32_t);
                    vga_puthex(val);
                    count++;
                    break;
                }
                case 's': {
                    const char* str = va_arg(args, const char*);
                    if (str) {
                        vga_puts(str);
                        count++;
                    }
                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);
                    vga_putchar(c);
                    count++;
                    break;
                }
                case '%':
                    vga_putchar('%');
                    count++;
                    break;
                default:
                    vga_putchar('%');
                    vga_putchar(*format);
                    count += 2;
                    break;
            }
        } else {
            vga_putchar(*format);
            count++;
        }
        format++;
    }
    
    va_end(args);
    return count;
}

// Enable the cursor
void vga_enable_cursor(void) {
    outb(VGA_CRTC_ADDR, VGA_CURSOR_START_REG);
    outb(VGA_CRTC_DATA, 0);

    outb(VGA_CRTC_ADDR, VGA_CURSOR_END_REG);
    outb(VGA_CRTC_DATA, 15);
}

// Disable the cursor
void vga_disable_cursor(void) {
    outb(VGA_CRTC_ADDR, VGA_CURSOR_START_REG);
    outb(VGA_CRTC_DATA, VGA_CURSOR_DISABLE);
}

// Update the cursor position with bounds checking
void vga_update_cursor(int x, int y) {
    if (!vga_buffer) return;
    if (!vga_bounds_check(x, y)) return;
    
    vga_column = (size_t)x;
    vga_row = (size_t)y;
    uint16_t pos = (uint16_t)(y * VGA_WIDTH + x);
    outb(VGA_CRTC_ADDR, VGA_CURSOR_LOW_REG);
    outb(VGA_CRTC_DATA, (uint8_t)(pos & 0xFF));
    outb(VGA_CRTC_ADDR, VGA_CURSOR_HIGH_REG);
    outb(VGA_CRTC_DATA, (uint8_t)((pos >> 8) & 0xFF));
}

// Set the current text color
void vga_set_color(vga_color_t fg, vga_color_t bg) {
    vga_color = vga_entry_color(fg, bg);
}

// Set color with attributes
void vga_set_color_attr(vga_color_t fg, vga_color_t bg, vga_attr_t attr) {
    vga_color = vga_entry_color_attr(fg, bg, attr);
}

// Get the current text color
uint8_t vga_get_color(void) {
    return vga_color;
}

// Move the cursor to a specific position
void vga_move_cursor(int x, int y) {
    if (vga_bounds_check(x, y)) {
        vga_column = (size_t)x;
        vga_row = (size_t)y;
        vga_update_cursor(x, y);
    }
}

// Get the current cursor position
void vga_get_cursor(int *x, int *y) {
    if (x) *x = (int)vga_column;
    if (y) *y = (int)vga_row;
}

// Swap double buffers
void vga_swap_buffers(void) {
    memcpy(VGA_MEMORY, vga_double_buffer, sizeof(vga_double_buffer));
}

// Set a custom buffer for all VGA output
void vga_set_buffer(uint16_t* buffer) {
    if (buffer)
        vga_buffer = buffer;
}

// Initialize the double buffer and set it as active
void vga_init_double_buffer(void) {
    vga_buffer = vga_double_buffer;
    vga_clear();
}

// Print a 32-bit hex number
void vga_puthex(uint32_t num) {
    static const char hex_chars[] = "0123456789ABCDEF";
    vga_puts("0x");
    for (int i = 28; i >= 0; i -= 4)
        vga_putchar(hex_chars[(num >> i) & 0xF]);
}

// Print a character at (x, y) without moving the cursor
void vga_putchar_at(char c, int x, int y) {
    if (!vga_buffer) return;
    if (vga_bounds_check(x, y))
        vga_buffer[y * VGA_WIDTH + x] = vga_entry(c, vga_color);
}

// Print a string at (x, y) without moving the global cursor
void vga_puts_at(const char *str, int x, int y) {
    if (!vga_buffer || !str) return;
    int orig_x = x;
    while (*str && y < VGA_HEIGHT) {
        if (*str == '\n') {
            y++;
            x = orig_x;
        } else if (vga_bounds_check(x, y)) {
            vga_putchar_at(*str, x, y);
            x++;
        }
        str++;
    }
}

// Fill an area with a character and color
void vga_fill_area(int x, int y, int width, int height, char c, uint8_t color) {
    if (!vga_buffer) return;
    
    uint16_t entry = vga_entry(c, color);
    for (int row = y; row < y + height && row < VGA_HEIGHT; row++) {
        for (int col = x; col < x + width && col < VGA_WIDTH; col++) {
            if (vga_bounds_check(col, row)) {
                vga_buffer[row * VGA_WIDTH + col] = entry;
            }
        }
    }
}

// Draw a simple box
void vga_draw_box(int x, int y, int width, int height, uint8_t color) {
    if (!vga_buffer || width < 2 || height < 2) return;
    
    // Draw corners and edges
    char corners[] = {'+', '+', '+', '+'};
    char edges[] = {'-', '|', '-', '|'};
    
    // Top and bottom edges
    for (int col = x; col < x + width; col++) {
        if (vga_bounds_check(col, y))
            vga_buffer[y * VGA_WIDTH + col] = vga_entry(
                (col == x || col == x + width - 1) ? corners[0] : edges[0], color);
        if (vga_bounds_check(col, y + height - 1))
            vga_buffer[(y + height - 1) * VGA_WIDTH + col] = vga_entry(
                (col == x || col == x + width - 1) ? corners[2] : edges[2], color);
    }
    
    // Left and right edges
    for (int row = y + 1; row < y + height - 1; row++) {
        if (vga_bounds_check(x, row))
            vga_buffer[row * VGA_WIDTH + x] = vga_entry(edges[1], color);
        if (vga_bounds_check(x + width - 1, row))
            vga_buffer[row * VGA_WIDTH + x + width - 1] = vga_entry(edges[3], color);
    }
}

// Save screen content to buffer
void vga_save_screen(uint16_t* buffer) {
    if (!buffer || !vga_buffer) return;
    memcpy(buffer, vga_buffer, sizeof(uint16_t) * VGA_HEIGHT * VGA_WIDTH);
}

// Restore screen content from buffer
void vga_restore_screen(const uint16_t* buffer) {
    if (!buffer || !vga_buffer) return;
    memcpy(vga_buffer, buffer, sizeof(uint16_t) * VGA_HEIGHT * VGA_WIDTH);
}

// Scroll screen up by specified lines
void vga_scroll_up(int lines) {
    vga_scroll_internal(lines);
    vga_update_cursor((int)vga_column, (int)vga_row);
}

// Scroll screen down by specified lines
void vga_scroll_down(int lines) {
    if (!vga_buffer || lines <= 0) return;
    
    if (lines >= VGA_HEIGHT) {
        vga_clear();
        return;
    }
    
    // Move lines down
    size_t move_size = sizeof(uint16_t) * VGA_WIDTH * (VGA_HEIGHT - lines);
    memmove(vga_buffer + (VGA_WIDTH * lines), vga_buffer, move_size);
    
    // Clear the top lines
    uint16_t blank = vga_entry(' ', vga_color);
    for (int line = 0; line < lines; line++) {
        for (int col = 0; col < VGA_WIDTH; col++) {
            vga_buffer[line * VGA_WIDTH + col] = blank;
        }
    }
    
    // Adjust cursor position
    vga_row = (vga_row + lines < VGA_HEIGHT) ? vga_row + lines : VGA_HEIGHT - 1;
    vga_update_cursor((int)vga_column, (int)vga_row);
}

// Get the length of text on a line (excluding trailing spaces)
int vga_get_line_length(int y) {
    if (!vga_buffer || y < 0 || y >= VGA_HEIGHT) return 0;
    
    int length = VGA_WIDTH;
    while (length > 0) {
        uint16_t entry = vga_buffer[y * VGA_WIDTH + length - 1];
        if ((entry & 0xFF) != ' ') break;
        length--;
    }
    return length;
}

// Delete a line and move lines below up
void vga_delete_line(int y) {
    if (!vga_buffer || y < 0 || y >= VGA_HEIGHT) return;
    
    // Move lines up
    if (y < VGA_HEIGHT - 1) {
        size_t move_size = sizeof(uint16_t) * VGA_WIDTH * (VGA_HEIGHT - y - 1);
        memmove(vga_buffer + y * VGA_WIDTH, 
                vga_buffer + (y + 1) * VGA_WIDTH, move_size);
    }
    
    // Clear the last line
    uint16_t blank = vga_entry(' ', vga_color);
    for (int col = 0; col < VGA_WIDTH; col++) {
        vga_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + col] = blank;
    }
}

// Insert a blank line and move lines below down
void vga_insert_line(int y) {
    if (!vga_buffer || y < 0 || y >= VGA_HEIGHT) return;
    
    // Move lines down
    if (y < VGA_HEIGHT - 1) {
        size_t move_size = sizeof(uint16_t) * VGA_WIDTH * (VGA_HEIGHT - y - 1);
        memmove(vga_buffer + (y + 1) * VGA_WIDTH,
                vga_buffer + y * VGA_WIDTH, move_size);
    }
    
    // Clear the inserted line
    uint16_t blank = vga_entry(' ', vga_color);
    for (int col = 0; col < VGA_WIDTH; col++) {
        vga_buffer[y * VGA_WIDTH + col] = blank;
    }
}
