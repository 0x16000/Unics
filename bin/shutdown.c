#include <vga.h>
#include <shell.h>
#include <stdint.h>
#include <stdbool.h>

// Simple delay function using a loop
static void delay(uint32_t count) {
    while (count--) {
        __asm__ volatile("nop");
    }
}

/**
 * Attempts a VirtualBox shutdown via the 0x4004 port.
 * This method is specific to VirtualBox.
 */
static bool try_vbox_shutdown() {
    vga_puts("Attempting VirtualBox shutdown...\n");

    // Write shutdown command to VirtualBox power management port (0x4004)
    __asm__ volatile (
        "outw %w0, %w1"
        :
        : "a" ((uint16_t)0x3400), "Nd" ((uint16_t)0x4004)
        : "memory"
    );

    // Wait to see if shutdown occurs
    delay(10000);
    return false;  // Return false if shutdown did not occur
}

/**
 * Attempts to initiate an ACPI shutdown via the 0x604 port.
 * Returns true if shutdown was initiated, false otherwise.
 */
static bool try_acpi_shutdown() {
    for (int attempt = 0; attempt < 3; attempt++) {
        // Write shutdown command to ACPI PM1a control register (0x604)
        __asm__ volatile (
            "outw %w0, %w1"
            :
            : "a" ((uint16_t)0x2000), "Nd" ((uint16_t)0x604)
            : "memory"
        );
        
        // Wait a bit to give time for the shutdown to take effect
        delay(10000);

        // If we're still here, the shutdown attempt failed
        vga_puts("ACPI shutdown attempt ");
        vga_putdec(attempt + 1, 1);  // Display attempt number
        vga_puts(" failed\n");
    }
    return false;  // Return false if ACPI shutdown failed after multiple attempts
}

/**
 * Attempts a Bochs/QEMU shutdown via the 0xB004 port.
 * This method is specific to Bochs and QEMU emulators.
 */
static bool try_bochs_shutdown() {
    vga_puts("Attempting Bochs/QEMU shutdown...\n");

    // Write shutdown command to Bochs/QEMU control port (0xB004)
    __asm__ volatile (
        "outw %w0, %w1"
        :
        : "a" ((uint16_t)0x2000), "Nd" ((uint16_t)0xB004)
        : "memory"
    );

    // Wait to see if shutdown occurs
    delay(10000);
    return false;  // Return false if shutdown did not occur
}

/**
 * Trigger a fallback reset by causing a triple fault.
 * This forces a system reset.
 */
static void system_fallback_reset() {
    vga_puts("Triggering triple fault to force reset...\n");

    // Create a null IDT descriptor (empty descriptor to trigger a fault)
    uint8_t idt_descriptor[6] = {0};

    // Load the null IDT
    __asm__ volatile (
        "lidt (%0)"
        :
        : "r" (idt_descriptor)
        : "memory"
    );

    // Trigger a divide-by-zero exception (this causes a triple fault)
    __asm__ volatile (
        "xor %%eax, %%eax\n\t"  // Set EAX to 0
        "div %%eax"             // Divide by zero, causing an exception
        :
        :
        : "eax", "memory"
    );

    // If we get here (which we shouldn't), halt the system
    __asm__ volatile ("hlt");
}

/**
 * Main entry point for the shutdown command.
 * Attempts various shutdown methods in order.
 * @param argc The argument count
 * @param argv The argument vector
 * @return int - status code: 0 for success, 1 for failure
 */
int shutdown_main(int argc, char **argv) {
    (void)argv;  // Explicitly mark unused parameter

    vga_puts("Attempting system shutdown...\n");

    // Check if the user passed any arguments (e.g., "-f" for forced shutdown)
    if (argc > 1 && argv[1][0] == '-') {
        vga_puts("Forcing shutdown...\n");
    }

    // Try VirtualBox shutdown first (if running in VirtualBox)
    if (try_vbox_shutdown()) {
        return 0;
    }

    // Try ACPI shutdown next
    if (try_acpi_shutdown()) {
        return 0;
    }

    // If ACPI shutdown failed, try Bochs/QEMU shutdown
    if (try_bochs_shutdown()) {
        return 0;
    }

    // If all shutdown methods failed, force a reset
    vga_puts("All shutdown methods failed, forcing reset...\n");
    system_fallback_reset();

    // We should never get here because the system should reset
    __asm__ volatile ("hlt");

    return 1;
}
