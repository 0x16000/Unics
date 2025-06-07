#include <io.h>
#include <keyboard.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

// Scancode to ASCII mapping (US QWERTY)
static const char kb_scancode_to_ascii[256] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-', 0, 0, 0, '+', 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Extended keys (0x80-0xFF)
    [0xE0] = 0, [0xE1] = 0, [0xE2] = 0
};

// Shift-scancode to ASCII mapping
static const char kb_shift_scancode_to_ascii[256] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-', 0, 0, 0, '+', 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Extended keys (0x80-0xFF)
    [0xE0] = 0, [0xE1] = 0, [0xE2] = 0
};

static kb_state_t kb_state = {0};

int kb_init(void) {
    // Initialize all state to false
    memset(&kb_state, 0, sizeof(kb_state));
    
    // Set default LED state
    kb_state.caps_lock = false;
    kb_state.num_lock = true;  // Num lock usually on by default
    kb_state.scroll_lock = false;
    
    // Update keyboard LEDs
    uint8_t leds = 0;
    if (kb_state.caps_lock) leds |= KB_LED_CAPS_LOCK;
    if (kb_state.num_lock) leds |= KB_LED_NUM_LOCK;
    if (kb_state.scroll_lock) leds |= KB_LED_SCROLL_LOCK;
    kb_set_leds(leds);
    
    return 0;
}

void kb_enable_input(bool enable) {
    kb_state.input_enabled = enable;
}

void kb_set_boot_complete(bool complete) {
    kb_state.boot_complete = complete;
}

void kb_set_leds(uint8_t led_state) {
    // Wait for keyboard to be ready
    while (inb(KB_STATUS_PORT) & 0x02);
    outb(KB_DATA_PORT, 0xED);  // LED command
    
    while (inb(KB_STATUS_PORT) & 0x02);
    outb(KB_DATA_PORT, led_state & 0x07);
    
    // Update our state
    kb_state.caps_lock = (led_state & KB_LED_CAPS_LOCK) != 0;
    kb_state.num_lock = (led_state & KB_LED_NUM_LOCK) != 0;
    kb_state.scroll_lock = (led_state & KB_LED_SCROLL_LOCK) != 0;
}

static char scancode_to_ascii(uint8_t scancode) {
    if ((uint16_t)scancode >= sizeof(kb_scancode_to_ascii)) return 0;
    
    bool shift = kb_state.shift_pressed;
    bool caps_lock = kb_state.caps_lock;
    
    // Handle letters with caps lock
    char base_char = kb_scancode_to_ascii[scancode];
    if (base_char >= 'a' && base_char <= 'z') {
        if (caps_lock) {
            shift = !shift;
        }
    }
    
    return shift ? kb_shift_scancode_to_ascii[scancode] 
                : kb_scancode_to_ascii[scancode];
}

static void handle_key_release(uint8_t scancode) {
    if (!kb_state.boot_complete) return;
    
    switch (scancode) {
        case 0x2A: // Left shift
            kb_state.left_shift_pressed = false;
            kb_state.shift_pressed = kb_state.left_shift_pressed || kb_state.right_shift_pressed;
            break;
        case 0x36: // Right shift
            kb_state.right_shift_pressed = false;
            kb_state.shift_pressed = kb_state.left_shift_pressed || kb_state.right_shift_pressed;
            break;
        case 0x1D: // Ctrl
            if (kb_state.extended_scancode) {
                kb_state.right_ctrl_pressed = false;
            } else {
                kb_state.left_ctrl_pressed = false;
            }
            kb_state.ctrl_pressed = kb_state.left_ctrl_pressed || kb_state.right_ctrl_pressed;
            break;
        case 0x38: // Alt
            if (kb_state.extended_scancode) {
                kb_state.right_alt_pressed = false;
            } else {
                kb_state.left_alt_pressed = false;
            }
            kb_state.alt_pressed = kb_state.left_alt_pressed || kb_state.right_alt_pressed;
            break;
    }
    kb_state.extended_scancode = false;
}

static void handle_key_press(uint8_t scancode) {
    if (!kb_state.boot_complete) return;
    
    switch (scancode) {
        case 0x2A: // Left shift
            kb_state.left_shift_pressed = true;
            kb_state.shift_pressed = true;
            break;
        case 0x36: // Right shift
            kb_state.right_shift_pressed = true;
            kb_state.shift_pressed = true;
            break;
        case 0x1D: // Ctrl
            if (kb_state.extended_scancode) {
                kb_state.right_ctrl_pressed = true;
            } else {
                kb_state.left_ctrl_pressed = true;
            }
            kb_state.ctrl_pressed = true;
            break;
        case 0x38: // Alt
            if (kb_state.extended_scancode) {
                kb_state.right_alt_pressed = true;
            } else {
                kb_state.left_alt_pressed = true;
            }
            kb_state.alt_pressed = true;
            break;
        case 0x3A: // Caps lock
            kb_state.caps_lock = !kb_state.caps_lock;
            kb_set_leds((kb_state.caps_lock ? KB_LED_CAPS_LOCK : 0) |
                       (kb_state.num_lock ? KB_LED_NUM_LOCK : 0) |
                       (kb_state.scroll_lock ? KB_LED_SCROLL_LOCK : 0));
            break;
        case 0x45: // Num lock
            kb_state.num_lock = !kb_state.num_lock;
            kb_set_leds((kb_state.caps_lock ? KB_LED_CAPS_LOCK : 0) |
                       (kb_state.num_lock ? KB_LED_NUM_LOCK : 0) |
                       (kb_state.scroll_lock ? KB_LED_SCROLL_LOCK : 0));
            break;
        case 0x46: // Scroll lock
            kb_state.scroll_lock = !kb_state.scroll_lock;
            kb_set_leds((kb_state.caps_lock ? KB_LED_CAPS_LOCK : 0) |
                       (kb_state.num_lock ? KB_LED_NUM_LOCK : 0) |
                       (kb_state.scroll_lock ? KB_LED_SCROLL_LOCK : 0));
            break;
    }
    kb_state.extended_scancode = false;
}

char kb_getchar(void) {
    if (!kb_state.input_enabled) return 0;
    
    uint8_t scancode;
    while (1) {
        // Wait for key press with small delay
        while ((inb(KB_STATUS_PORT) & 0x01) == 0) {
            asm volatile("pause");
        }
        
        scancode = inb(KB_DATA_PORT);

        // Handle extended scancode prefix
        if (scancode == KB_SCANCODE_EXTENDED) {
            kb_state.extended_scancode = true;
            continue;
        }

        // Handle key release
        if (scancode & KB_SCANCODE_RELEASE) {
            handle_key_release(scancode & ~KB_SCANCODE_RELEASE);
            continue;
        }
        
        // Handle key press
        handle_key_press(scancode);
        
        // Convert to ASCII
        char c = scancode_to_ascii(scancode);
        if (c != 0) return c;
    }
}

bool kb_check_escape(void) {
    if (!(inb(KB_STATUS_PORT) & 0x01)) return false;
    uint8_t scancode = inb(KB_DATA_PORT);
    return (scancode == KB_SCANCODE_ESC);
}

void kb_flush(void) {
    while (inb(KB_STATUS_PORT) & 0x01) {
        inb(KB_DATA_PORT);
    }
}

bool kb_shift_pressed(void) { return kb_state.shift_pressed; }
bool kb_ctrl_pressed(void) { return kb_state.ctrl_pressed; }
bool kb_alt_pressed(void) { return kb_state.alt_pressed; }
bool kb_caps_lock_on(void) { return kb_state.caps_lock; }
bool kb_num_lock_on(void) { return kb_state.num_lock; }
bool kb_scroll_lock_on(void) { return kb_state.scroll_lock; }
bool kb_is_extended(uint8_t scancode) { return scancode == KB_SCANCODE_EXTENDED; }
bool kb_is_release(uint8_t scancode) { return scancode & KB_SCANCODE_RELEASE; }
