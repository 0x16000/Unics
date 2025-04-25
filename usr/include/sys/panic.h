#ifndef _PANIC_H
#define _PANIC_H

#include <stdnoreturn.h>
#include <stdint.h>

/**
 * Kernel panic function - halts the system on critical errors
 * @param message: Error description to display
 * @param file:    Source file where panic occurred (use __FILE__)
 * @param line:    Line number (use __LINE__)
 */
noreturn void panic(const char *message, const char *file, uint32_t line);

/* Macro for convenient panic invocation */
#define PANIC(msg) panic(msg, __FILE__, __LINE__)

/* Halt CPU indefinitely */
void panic_halt(void);

#endif /* _PANIC_H */
