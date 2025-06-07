#include <vga.h>
#include <io.h>
#include <string.h>

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

// Double buffer (optional)
static uint16_t vga_double_buffer[VGA_HEIGHT * VGA_WIDTH];

// Helper: scroll the screen up by one line
static void vga_scroll(void) {
    // Move all lines up
    memmove(
        vga_buffer,
        vga_buffer + VGA_WIDTH,
        sizeof(uint16_t) * VGA_WIDTH * (VGA_HEIGHT - 1)
    );
    // Clear last line
    uint16_t blank = vga_entry(' ', vga_color);
    for (size_t x = 0; x < VGA_WIDTH; x++)
        vga_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = blank;
    if (vga_row > 0) vga_row--;
    vga_column = 0;
}

// Initialize VGA
int vga_initialize(void) {
    vga_color = VGA_COLOR_DEFAULT;
    vga_buffer = VGA_MEMORY;
    vga_row = 0;
    vga_column = 0;
    vga_clear();
    vga_enable_cursor();
    return 0;
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

// Print a character
void vga_putchar(char c) {
    if (!vga_buffer) return;

    switch (c) {
    case '\n':
        vga_column = 0;
        if (++vga_row >= VGA_HEIGHT)
            vga_scroll();
        break;
    case '\b':
        if (vga_column > 0) {
           vga_column--;
    }   else if (vga_row > 0) {
           vga_row--;
           vga_column = VGA_WIDTH - 1;
    }
    vga_buffer[vga_row * VGA_WIDTH + vga_column] = vga_entry(' ', vga_color);
    vga_update_cursor(vga_column, vga_row);  // Add this line
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
            vga_scroll();
    }
    break;
}
    default:
        if (c >= ' ' && c <= '~') {
            vga_buffer[vga_row * VGA_WIDTH + vga_column] = vga_entry(c, vga_color);
            if (++vga_column >= VGA_WIDTH) {
                vga_column = 0;
                if (++vga_row >= VGA_HEIGHT)
                    vga_scroll();
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
    do {
        buffer[i++] = '0' + (value % 10);
        value /= 10;
    } while (value && i < (int)sizeof(buffer) - 1);

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

// Update the cursor position
void vga_update_cursor(int x, int y) {
    if (!vga_buffer) return;
    if (x < 0 || x >= VGA_WIDTH || y < 0 || y >= VGA_HEIGHT)
        return;
    vga_column = x;
    vga_row = y;
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

// Get the current text color
uint8_t vga_get_color(void) {
    return vga_color;
}

// Move the cursor to a specific position
void vga_move_cursor(int x, int y) {
    if (x >= 0 && x < (int)VGA_WIDTH && y >= 0 && y < (int)VGA_HEIGHT) {
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

// Swap double buffers (copy double buffer to video memory)
void vga_swap_buffers(void) {
    memcpy(VGA_MEMORY, vga_double_buffer, sizeof(vga_double_buffer));
}

// Set a custom buffer for all VGA output.
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
    if (x >= 0 && x < (int)VGA_WIDTH && y >= 0 && y < (int)VGA_HEIGHT)
        vga_buffer[y * VGA_WIDTH + x] = vga_entry(c, vga_color);
}

// Print a string at (x, y) without moving the global cursor
void vga_puts_at(const char *str, int x, int y) {
    if (!vga_buffer || !str) return;
    int orig_x = x;
    while (*str && y < (int)VGA_HEIGHT) {
        if (*str == '\n') {
            y++;
            x = orig_x;
        } else if (x >= 0 && x < (int)VGA_WIDTH) {
            vga_putchar_at(*str, x, y);
            x++;
        }
        str++;
    }
}
