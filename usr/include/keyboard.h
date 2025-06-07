#ifndef KB_H
#define KB_H

#include <stdint.h>
#include <stdbool.h>

// Port definitions
#define KB_DATA_PORT    0x60
#define KB_STATUS_PORT  0x64
#define KB_CMD_PORT     0x64

// Special scancodes
#define KB_SCANCODE_ESC       0x01
#define KB_SCANCODE_RELEASE   0x80
#define KB_SCANCODE_EXTENDED  0xE0

// LED state bits
#define KB_LED_SCROLL_LOCK   (1 << 0)
#define KB_LED_NUM_LOCK      (1 << 1)
#define KB_LED_CAPS_LOCK     (1 << 2)

// Modifier key states
typedef struct {
    bool shift_pressed;
    bool ctrl_pressed;
    bool alt_pressed;
    bool caps_lock;
    bool num_lock;
    bool scroll_lock;
    bool input_enabled;
    bool boot_complete;
    bool left_shift_pressed;
    bool right_shift_pressed;
    bool left_ctrl_pressed;
    bool right_ctrl_pressed;
    bool left_alt_pressed;
    bool right_alt_pressed;
    bool extended_scancode;
} kb_state_t;

// Initialization
int kb_init(void);

// State control
void kb_enable_input(bool enable);
void kb_set_boot_complete(bool complete);
void kb_set_leds(uint8_t led_state);

// Key reading
char kb_getchar(void);
void kb_flush(void);

// State queries
bool kb_check_escape(void);
bool kb_shift_pressed(void);
bool kb_ctrl_pressed(void);
bool kb_alt_pressed(void);
bool kb_caps_lock_on(void);
bool kb_num_lock_on(void);
bool kb_scroll_lock_on(void);

// Special key handling
bool kb_is_extended(uint8_t scancode);
bool kb_is_release(uint8_t scancode);

#endif // KB_H
