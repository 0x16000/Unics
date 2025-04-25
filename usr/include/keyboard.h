#ifndef KB_H
#define KB_H

#include <stdint.h>
#include <stdbool.h>

#define KB_DATA_PORT 0x60
#define KB_STATUS_PORT 0x64

typedef struct {
    bool shift_pressed;      // Either shift key is pressed
    bool ctrl_pressed;
    bool alt_pressed;
    bool caps_lock;
    bool input_enabled;
    bool boot_complete;
    bool left_shift_pressed; // Track left shift separately
    bool right_shift_pressed; // Track right shift separately
} kb_state_t;

bool kb_check_escape(void);
void kb_flush(void);
bool kb_ctrl_pressed(void);

// Keyboard initialization
int kb_init(void);

// Input control
void kb_enable_input(bool enable);
void kb_set_boot_complete(bool complete);

// Key reading
char kb_getchar(void);

#endif // KB_H
