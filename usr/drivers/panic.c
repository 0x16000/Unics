#include <sys/panic.h>
#include <vga.h>
#include <arch/i386/cpu.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

/* Panic colors */
#define PANIC_FG VGA_COLOR_WHITE
#define PANIC_BG VGA_COLOR_RED
#define HEADER_FG VGA_COLOR_YELLOW
#define HEADER_BG VGA_COLOR_RED

/* Panic screen layout */
#define PANIC_HEADER " KERNEL PANIC "
#define PANIC_WIDTH 80
#define PANIC_DIVIDER "======================================================="

noreturn void panic(const char *message, const char *file, uint32_t line) {
    /* Setup panic screen */
    vga_set_color(PANIC_FG, PANIC_BG);
    vga_clear();

    /* Draw header */
    vga_set_color(HEADER_FG, HEADER_BG);
    vga_puts_at(PANIC_HEADER, (PANIC_WIDTH - strlen(PANIC_HEADER)) / 2, 0);

    /* Main error message */
    vga_set_color(PANIC_FG, PANIC_BG);
    vga_puts_at("ERROR: ", 2, 2);
    vga_puts_at(message, 9, 2);

    /* Location info */
    vga_puts_at("File: ", 2, 4);
    vga_puts_at(file, 8, 4);
    
    char line_str[16];
    itoa(line, line_str, 10);
    vga_puts_at("Line: ", 2, 5);
    vga_puts_at(line_str, 8, 5);

    /* Divider */
    vga_puts_at(PANIC_DIVIDER, (PANIC_WIDTH - strlen(PANIC_DIVIDER)) / 2, 7);

    /* System state info */
    vga_puts_at("System halted. No safe recovery possible.", 2, 9);

    /* Halt the system */
    panic_halt();
}

void panic_halt(void) {
    asm volatile ("cli");
    for (;;) {
        asm volatile ("hlt");
    }
}
