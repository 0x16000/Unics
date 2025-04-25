#ifndef _BOOTSCREEN_H
#define _BOOTSCREEN_H

#include <vga.h>

/* Public API */
void bootscreen_init(void);
void bootscreen_clear(void);
void bootscreen_draw_logo(void);
void bootscreen_write_centered(int y, const char *str);
void bootscreen_write_status(const char *status);
void bootscreen_progress(int percentage);
void bootscreen_error(const char *message);

#endif /* _BOOTSCREEN_H */
