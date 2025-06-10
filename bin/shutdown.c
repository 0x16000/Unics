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
        : "a" ((uint16_t)0x2000), "Nd" ((uint16_t)0x4004)  // Fixed: Changed from 0x3400 to 0x2000
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
    vga_puts("Attempting ACPI shutdown...\n");  // Added status message
    
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
 * Try to shutdown via ACPI by reading DSDT table (more proper method)
 * This is a placeholder for a more complete ACPI implementation
 */
static bool try_proper_acpi_shutdown() {
    // This would require parsing ACPI tables to find the correct PM1a_CNT register
    // For now, we'll try common ACPI ports
    uint16_t acpi_ports[] = {0x1004, 0x604, 0x404, 0};
    
    for (int i = 0; acpi_ports[i] != 0; i++) {
        vga_puts("Trying ACPI port 0x");
        // You'd need a hex print function here
        vga_puts("...\n");
        
        __asm__ volatile (
            "outw %w0, %w1"
            :
            : "a" ((uint16_t)0x2000), "Nd" (acpi_ports[i])
            : "memory"
        );
        
        delay(5000);
    }
    
    return false;
}

/**
 * Trigger a fallback reset by causing a triple fault.
 * This forces a system reset.
 */
static void system_fallback_reset() {
    vga_puts("Triggering triple fault to force reset...\n");

    // Disable interrupts first
    __asm__ volatile ("cli");

    // Create a null IDT descriptor (limit=0, base=0)
    struct {
        uint16_t limit;
        uint32_t base;
    } __attribute__((packed)) null_idt = {0, 0};

    // Load the null IDT
    __asm__ volatile (
        "lidt %0"
        :
        : "m" (null_idt)
        : "memory"
    );

    // Trigger an interrupt to cause a triple fault
    __asm__ volatile ("int $0x03");

    // Alternative: divide by zero
    __asm__ volatile (
        "xor %%eax, %%eax\n\t"
        "div %%eax"
        :
        :
        : "eax"
    );

    // If we get here (which we shouldn't), halt the system
    for(;;) {
        __asm__ volatile ("hlt");
    }
}

/**
 * Main entry point for the shutdown command.
 */
int shutdown_main(int argc, char **argv) {
    (void)argv;  // Explicitly mark unused parameter

    vga_puts("Attempting system shutdown...\n");

    // Check if the user passed any arguments
    bool force_shutdown = false;
    if (argc > 1 && argv[1] != NULL && argv[1][0] == '-') {  // Added null check
        if (argv[1][1] == 'f') {  // Check for -f flag specifically
            vga_puts("Forcing shutdown...\n");
            force_shutdown = true;
        }
    }

    // Try VirtualBox shutdown first
    if (try_vbox_shutdown()) {
        return 0;
    }

    // Try proper ACPI shutdown
    if (try_proper_acpi_shutdown()) {
        return 0;
    }

    // Try original ACPI method
    if (try_acpi_shutdown()) {
        return 0;
    }

    // Try Bochs/QEMU shutdown
    if (try_bochs_shutdown()) {
        return 0;
    }

    // If all shutdown methods failed, decide what to do
    if (force_shutdown) {
        vga_puts("All shutdown methods failed, forcing reset...\n");
        system_fallback_reset();
    } else {
        vga_puts("All shutdown methods failed. Use 'shutdown -f' to force reset.\n");
        return 1;
    }

    // We should never get here
    for(;;) {
        __asm__ volatile ("hlt");
    }
}
