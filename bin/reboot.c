#include <shell.h>
#include <vga.h>
#include <stdint.h>
#include <io.h>

/**
 * Attempts a VirtualBox reboot via the 0x4004 port.
 * This method is specific to VirtualBox.
 */
static bool try_vbox_reboot() {
    vga_puts("Attempting VirtualBox reboot...\n");

    // Write reboot command to VirtualBox power management port (0x4004)
    __asm__ volatile (
        "outw %w0, %w1"
        :
        : "a" ((uint16_t)0x3400), "Nd" ((uint16_t)0x4004)
        : "memory"
    );

    // Wait to see if reboot occurs
    for (int i = 0; i < 1000000; i++) {
        io_wait();
    }
    return false;  // Return false if reboot did not occur
}

/**
 * Force a system reset by triggering a triple fault.
 * This is a last-resort fallback mechanism.
 */
static void system_fallback_reset(void) {
    vga_puts("Attempting triple fault reset...\n");

    // Disable interrupts
    __asm__ volatile ("cli");

    // Create a null IDT descriptor (empty IDT entry)
    uint8_t idt_descriptor[6] = {0};

    // Load the null IDT descriptor to trigger the fault
    __asm__ volatile ("lidt (%0)" : : "r" (idt_descriptor) : "memory");

    // Trigger a triple fault by causing an exception
    __asm__ volatile ("int $0");

    // If still here, try dividing by zero to trigger another fault
    volatile int zero = 0;
    __asm__ volatile ("div %0" : : "r"(zero));

    // If we get to this point, just halt the system indefinitely
    for(;;) {
        __asm__ volatile ("hlt");
    }
}

/**
 * Try to reboot the system using the 8042 keyboard controller reset command.
 * This method works on systems with a working 8042 controller.
 */
static bool try_8042_reset(void) {
    vga_puts("Attempting 8042 controller reset...\n");

    uint8_t status;
    int timeout = 500000;  // Timeout value for waiting

    // Disable interrupts while interacting with the 8042 controller
    __asm__ volatile ("cli");

    // Wait until the 8042 controller is ready to accept a command
    do {
        __asm__ volatile ("inb $0x64, %0" : "=a"(status));
        io_wait();
        if (--timeout == 0) {
            vga_puts("8042 controller not responding.\n");
            return false;
        }
    } while (status & 0x02);  // Wait for input buffer to be empty

    // Send the reset command (0xFE) to the 8042 controller
    __asm__ volatile ("outb %0, $0x64" : : "a"((uint8_t)0xFE));
    io_wait();

    vga_puts("Reset command sent to 8042 controller.\n");

    // Allow some time for the reset to take effect
    for (int i = 0; i < 1000000; i++) {
        io_wait();
    }

    return true;
}

/**
 * Main entry point for the reboot command.
 * Attempts various methods to reboot the system.
 * @param argc The number of command-line arguments.
 * @param argv The command-line arguments.
 * @return int Returns 0 on success, 1 on failure.
 */
int reboot_main(int argc, char **argv) {
    (void)argv;  // Unused parameter, marked for clarity

    vga_puts("Initiating system reboot...\n");

    // Check for any command-line options (e.g., -f for forced reboot)
    if (argc > 1 && argv[1][0] == '-') {
        vga_puts("Forced reboot option detected...\n");
    }

    // Try VirtualBox reboot first
    if (try_vbox_reboot()) {
        return 0;
    }

    // Try the 8042 controller reset next
    if (try_8042_reset()) {
        return 0;
    }

    // If the 8042 reset failed, attempt the fallback reset method
    vga_puts("Primary reset methods failed, trying fallback...\n");
    system_fallback_reset();

    // We should never reach here as the system will be reset or halted
    vga_puts("All reboot methods failed! System halted.\n");
    for(;;) {
        __asm__ volatile ("hlt");
    }

    return 1;
}
