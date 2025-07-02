#include <io.h>
#include <keyboard.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

static const char kb_scancode_to_ascii[256] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-', 0, 0, 0, '+', 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    [0xE0] = 0, [0xE1] = 0, [0xE2] = 0
};

static const char kb_shift_scancode_to_ascii[256] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-', 0, 0, 0, '+', 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    [0xE0] = 0, [0xE1] = 0, [0xE2] = 0
};

static kb_state_t kb_state = {0};

// Helper: Wait until input buffer empty (ready to write command)
static bool wait_input_buffer_empty(void) {
    const uint16_t max_retries = 10000; // increased retry count for robustness
    for (uint16_t i = 0; i < max_retries; i++) {
        if ((inb(KB_STATUS_PORT) & 0x02) == 0) return true;
        asm volatile("pause");
    }
    return false;
}

// Helper: Wait for ACK from keyboard
static bool wait_for_ack(void) {
    const uint16_t max_retries = 10000;
    for (uint16_t i = 0; i < max_retries; i++) {
        if (inb(KB_STATUS_PORT) & 0x01) {
            if (inb(KB_DATA_PORT) == 0xFA) return true;
        }
        asm volatile("pause");
    }
    return false;
}

int kb_init(void) {
    memset(&kb_state, 0, sizeof(kb_state));
    kb_flush();

    kb_state.caps_lock = false;
    kb_state.num_lock = true;  // Usually on by default
    kb_state.scroll_lock = false;

    // Update LEDs once
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

// Consolidated LED update to reduce duplication
static void update_leds(void) {
    uint8_t leds = 0;
    if (kb_state.caps_lock) leds |= KB_LED_CAPS_LOCK;
    if (kb_state.num_lock) leds |= KB_LED_NUM_LOCK;
    if (kb_state.scroll_lock) leds |= KB_LED_SCROLL_LOCK;
    kb_set_leds(leds);
}

void kb_set_leds(uint8_t led_state) {
    led_state &= (KB_LED_SCROLL_LOCK | KB_LED_NUM_LOCK | KB_LED_CAPS_LOCK);

    if (!wait_input_buffer_empty()) return;
    outb(KB_CMD_PORT, 0xED); // LED command

    if (!wait_for_ack()) return;

    if (!wait_input_buffer_empty()) return;
    outb(KB_DATA_PORT, led_state);

    if (!wait_for_ack()) return;

    // Update internal state on success
    kb_state.caps_lock = (led_state & KB_LED_CAPS_LOCK) != 0;
    kb_state.num_lock = (led_state & KB_LED_NUM_LOCK) != 0;
    kb_state.scroll_lock = (led_state & KB_LED_SCROLL_LOCK) != 0;
}

static char scancode_to_ascii(uint8_t scancode) {
    if (scancode >= sizeof(kb_scancode_to_ascii)) return 0;

    bool shift = kb_state.shift_pressed;
    bool caps = kb_state.caps_lock;

    char base_char = kb_scancode_to_ascii[scancode];

    // Only toggle shift if letter a-z and caps lock is on
    if (base_char >= 'a' && base_char <= 'z' && caps) {
        shift = !shift;
    }

    return shift ? kb_shift_scancode_to_ascii[scancode] : base_char;
}

static void update_modifier(bool pressed, bool *left, bool *right, bool *modifier, bool extended) {
    if (extended) {
        *right = pressed;
    } else {
        *left = pressed;
    }
    *modifier = *left || *right;
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
            update_modifier(false, &kb_state.left_ctrl_pressed, &kb_state.right_ctrl_pressed, &kb_state.ctrl_pressed, kb_state.extended_scancode);
            break;
        case 0x38: // Alt
            update_modifier(false, &kb_state.left_alt_pressed, &kb_state.right_alt_pressed, &kb_state.alt_pressed, kb_state.extended_scancode);
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
            update_modifier(true, &kb_state.left_ctrl_pressed, &kb_state.right_ctrl_pressed, &kb_state.ctrl_pressed, kb_state.extended_scancode);
            break;
        case 0x38: // Alt
            update_modifier(true, &kb_state.left_alt_pressed, &kb_state.right_alt_pressed, &kb_state.alt_pressed, kb_state.extended_scancode);
            break;
        case 0x3A: // Caps lock
            kb_state.caps_lock = !kb_state.caps_lock;
            update_leds();
            break;
        case 0x45: // Num lock
            kb_state.num_lock = !kb_state.num_lock;
            update_leds();
            break;
        case 0x46: // Scroll lock
            kb_state.scroll_lock = !kb_state.scroll_lock;
            update_leds();
            break;
    }
    kb_state.extended_scancode = false;
}

char kb_getchar(void) {
    if (!kb_state.input_enabled) return 0;

    while (1) {
        while ((inb(KB_STATUS_PORT) & 0x01) == 0) {
            asm volatile("pause");
        }

        uint8_t scancode = inb(KB_DATA_PORT);

        if (scancode == KB_SCANCODE_EXTENDED) {
            kb_state.extended_scancode = true;
            continue;
        }

        if (scancode & KB_SCANCODE_RELEASE) {
            handle_key_release(scancode & ~KB_SCANCODE_RELEASE);
            continue;
        }

        handle_key_press(scancode);

        char c = scancode_to_ascii(scancode);
        if (c != 0) return c;
    }
}

bool kb_check_escape(void) {
    if (!(inb(KB_STATUS_PORT) & 0x01)) return false;
    return inb(KB_DATA_PORT) == KB_SCANCODE_ESC;
}

void kb_flush(void) {
    while (inb(KB_STATUS_PORT) & 0x01) {
        (void)inb(KB_DATA_PORT);
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
