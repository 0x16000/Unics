#include <vga.h>
#include <io.h>
#include <string.h>

// VGA memory address
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;

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

// Current cursor position and color
static size_t vga_row;
static size_t vga_column;
static uint8_t vga_color;
static uint16_t* vga_buffer;

// Double buffer (optional)
static uint16_t vga_double_buffer[VGA_HEIGHT * VGA_WIDTH];

// Scroll the screen up by one line
static void vga_scroll(void) {
    for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            vga_buffer[y * VGA_WIDTH + x] = vga_buffer[(y + 1) * VGA_WIDTH + x];
        }
    }

    // Clear the bottom line
    uint16_t blank = vga_entry(' ', vga_color);
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        vga_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = blank;
    }

    vga_row = VGA_HEIGHT - 1;
    vga_column = 0;
}

// Initialize VGA
int vga_initialize(void) {
    vga_row = 0;
    vga_column = 0;
    vga_color = VGA_COLOR_DEFAULT;
    vga_buffer = VGA_MEMORY; // Default to VGA memory
    vga_clear();
    vga_enable_cursor();
    return 0;
}

// Clear the screen
void vga_clear(void) {
    uint16_t blank = vga_entry(' ', vga_color);
    for (size_t i = 0; i < VGA_HEIGHT * VGA_WIDTH; i++) {
        vga_buffer[i] = blank;
    }
    vga_row = 0;
    vga_column = 0;
    vga_update_cursor(vga_column, vga_row);
}

// Print a character
void vga_putchar(char c) {
    if (c == '\n') {
        vga_column = 0;
        if (++vga_row == VGA_HEIGHT) {
            vga_scroll();
        }
    } else if (c == '\b') {
        if (vga_column > 0) {
            vga_column--;
        } else if (vga_row > 0) {
            vga_row--;
            vga_column = VGA_WIDTH - 1;
        } else {
            // Cursor is at the top-left corner; do nothing
            return;
        }
        size_t index = vga_row * VGA_WIDTH + vga_column;
        vga_buffer[index] = vga_entry(' ', vga_color);
    } else if (c == '\t') {
        vga_column = (vga_column + TAB_WIDTH) & ~(TAB_WIDTH - 1); // Align to tab width
        if (vga_column >= VGA_WIDTH) {
            vga_column = 0;
            if (++vga_row == VGA_HEIGHT) {
                vga_scroll();
            }
        }
    } else if (c >= ' ' && c <= '~') { // Only printable ASCII characters
        size_t index = vga_row * VGA_WIDTH + vga_column;
        vga_buffer[index] = vga_entry(c, vga_color);
        if (++vga_column == VGA_WIDTH) {
            vga_column = 0;
            if (++vga_row == VGA_HEIGHT) {
                vga_scroll();
            }
        }
    }
    vga_update_cursor(vga_column, vga_row);
}

void vga_putdec(uint32_t value, uint8_t digits) {
    char buffer[10]; // Maximum 10 digits for a 32-bit number
    int i = 0;

    // Convert number to string
    do {
        buffer[i++] = '0' + (value % 10);
        value /= 10;
    } while (value > 0);

    // Pad with leading zeros if necessary
    while (i < digits) {
        buffer[i++] = '0';
    }

    // Print in reverse order
    while (i > 0) {
        vga_putchar(buffer[--i]);
    }
}

// Print a string
void vga_puts(const char* str) {
    if (!str) return; // Null pointer check
    while (*str) {
        vga_putchar(*str++);
    }
}

// Enable the cursor
void vga_enable_cursor(void) {
    outb(VGA_CRTC_ADDR, VGA_CURSOR_START_REG);
    outb(VGA_CRTC_DATA, 14); // Cursor starts at scanline 14

    outb(VGA_CRTC_ADDR, VGA_CURSOR_END_REG);
    outb(VGA_CRTC_DATA, 15); // Cursor ends at scanline 15
}

// Disable the cursor
void vga_disable_cursor(void) {
    outb(VGA_CRTC_ADDR, VGA_CURSOR_START_REG);
    outb(VGA_CRTC_DATA, VGA_CURSOR_DISABLE);
}

// Update the cursor position
void vga_update_cursor(int x, int y) {
    if (x < 0 || x >= VGA_WIDTH || y < 0 || y >= VGA_HEIGHT) {
        return; // Invalid position
    }
    uint16_t pos = y * VGA_WIDTH + x;

    outb(VGA_CRTC_ADDR, VGA_CURSOR_LOW_REG);
    outb(VGA_CRTC_DATA, (uint8_t) (pos & 0xFF));

    outb(VGA_CRTC_ADDR, VGA_CURSOR_HIGH_REG);
    outb(VGA_CRTC_DATA, (uint8_t) ((pos >> 8) & 0xFF));
}

// Set the current text color
void vga_set_color(enum vga_color fg, enum vga_color bg) {
    vga_color = vga_entry_color(fg, bg);
}

// Get the current text color
uint8_t vga_get_color(void) {
    return vga_color;
}

// Move the cursor to a specific position
void vga_move_cursor(int x, int y) {
    if (x >= 0 && x < VGA_WIDTH && y >= 0 && y < VGA_HEIGHT) {
        vga_column = x;
        vga_row = y;
        vga_update_cursor(vga_column, vga_row);
    }
}

// Get the current cursor position
void vga_get_cursor(int *x, int *y) {
    *x = vga_column;
    *y = vga_row;
}

// Swap double buffers
void vga_swap_buffers(void) {
    memcpy(VGA_MEMORY, vga_double_buffer, VGA_HEIGHT * VGA_WIDTH * sizeof(uint16_t));
}

// Set a custom buffer
void vga_set_buffer(uint16_t* buffer) {
    vga_buffer = buffer;
}

// Initialize the double buffer
void vga_init_double_buffer(void) {
    vga_buffer = vga_double_buffer;
    vga_clear();
}

void vga_puthex(uint32_t num) {
    const char hex_chars[] = "0123456789ABCDEF";
    vga_puts("0x");
    
    // Print each nibble starting from the most significant
    for (int i = 28; i >= 0; i -= 4) {
        uint8_t nibble = (num >> i) & 0xF;
        vga_putchar(hex_chars[nibble]);
    }
}

void vga_putchar_at(char c, int x, int y) {
    if (x >= 0 && x < VGA_WIDTH && y >= 0 && y < VGA_HEIGHT) {
        size_t index = y * VGA_WIDTH + x;
        vga_buffer[index] = vga_entry(c, vga_color);
    }
}

void vga_puts_at(const char *str, int x, int y) {
    int original_x = x;
    while (*str) {
        if (*str == '\n') {
            y++;
            x = original_x;
        } else {
            vga_putchar_at(*str, x++, y);
        }
        str++;
    }
}
