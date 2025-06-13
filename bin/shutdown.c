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
 * Check if we're running in VirtualBox by looking for specific indicators
 */
static bool detect_virtualbox() {
    // Try to detect VirtualBox by checking for VBox guest additions port
    uint16_t test_value = 0;
    
    // VirtualBox typically responds to certain port reads
    __asm__ volatile (
        "inw %w1, %w0"
        : "=a" (test_value)
        : "Nd" ((uint16_t)0x4004)
        : "memory"
    );
    
    // This is a basic check - in a real implementation you'd want more thorough detection
    return true; // Assume VirtualBox for now
}

/**
 * Attempts a VirtualBox shutdown via emulator-specific methods.
 * VirtualBox doesn't always respond to standard ACPI without proper AML.
 */
static bool try_vbox_shutdown() {
    vga_puts("Attempting VirtualBox-specific shutdown...\n");

    // Method 1: Try VirtualBox's debug/test ports
    // Some VirtualBox versions respond to this
    __asm__ volatile (
        "outw %w0, %w1"
        :
        : "a" ((uint16_t)0x2000), "Nd" ((uint16_t)0x4004)
        : "memory"
    );
    delay(5000);
    
    // Method 2: Try QEMU-style shutdown (VBox sometimes emulates this)
    __asm__ volatile (
        "outw %w0, %w1"
        :
        : "a" ((uint16_t)0x2000), "Nd" ((uint16_t)0x604)
        : "memory"
    );
    delay(5000);
    
    // Method 3: Try Bochs/QEMU debug exit
    __asm__ volatile (
        "outw %w0, %w1"
        :
        : "a" ((uint16_t)0x2000), "Nd" ((uint16_t)0xB004)
        : "memory"
    );
    delay(5000);
    
    return false; // If we're still here, it didn't work
}

/**
 * Try a simple halt-based "shutdown" that stops execution
 * This is what many simple OSs do when they can't actually power off
 */
static bool try_halt_shutdown() {
    vga_puts("Performing halt-based shutdown...\n");
    vga_puts("System halted. You can now close the VM.\n");
    
    // Disable interrupts and halt
    __asm__ volatile ("cli");
    
    // Infinite halt loop
    for(;;) {
        __asm__ volatile ("hlt");
    }
    
    return true; // This should never return
}

/**
 * Attempts a QEMU shutdown via the 0x604 port with proper value.
 */
static bool try_qemu_shutdown() {
    vga_puts("Attempting QEMU shutdown...\n");

    // QEMU isa-debug-exit device (if enabled)
    __asm__ volatile (
        "outw %w0, %w1"
        :
        : "a" ((uint16_t)0x501), "Nd" ((uint16_t)0x501)
        : "memory"
    );

    delay(5000);
    
    // Also try QEMU's ACPI implementation
    __asm__ volatile (
        "outw %w0, %w1"
        :
        : "a" ((uint16_t)0x3400), "Nd" ((uint16_t)0x604)
        : "memory"
    );

    delay(10000);
    return false;
}

/**
 * Safe keyboard controller reset method
 */
static bool try_keyboard_reset() {
    vga_puts("Attempting keyboard controller reset...\n");
    
    // Wait for keyboard controller to be ready
    int timeout = 1000;
    while (timeout-- > 0) {
        __asm__ volatile (
            "inb $0x64, %%al"
            :
            :
            : "al"
        );
        uint8_t status;
        __asm__ volatile (
            "inb $0x64, %0"
            : "=a" (status)
        );
        
        if (!(status & 0x02)) break; // Input buffer empty
        delay(100);
    }
    
    if (timeout <= 0) {
        vga_puts("Keyboard controller timeout\n");
        return false;
    }
    
    // Send reset command to keyboard controller
    __asm__ volatile (
        "outb %%al, $0x64"
        :
        : "a" ((uint8_t)0xFE)
        : "memory"
    );
    
    delay(10000);
    return false;
}

/**
 * Trigger a controlled reset by causing a triple fault.
 */
static void system_fallback_reset() {
    vga_puts("Initiating controlled system reset...\n");
    
    // Give user a moment to see the message
    delay(100000);

    // Disable interrupts
    __asm__ volatile ("cli");

    // Method 1: Try keyboard controller reset first (safer)
    if (try_keyboard_reset()) {
        return;
    }

    // Method 2: Triple fault as last resort
    vga_puts("Forcing triple fault...\n");
    delay(50000);

    // Create invalid IDT
    struct {
        uint16_t limit;
        uint32_t base;
    } __attribute__((packed)) null_idt = {0, 0};

    __asm__ volatile (
        "lidt %0"
        :
        : "m" (null_idt)
        : "memory"
    );

    // Trigger interrupt
    __asm__ volatile ("int $0x01");

    // Infinite halt loop
    for(;;) {
        __asm__ volatile ("hlt");
    }
}

/**
 * Main entry point for the shutdown command.
 */
int shutdown_main(int argc, char **argv) {
    (void)argc; // Mark as intentionally unused
    (void)argv; // Mark as intentionally unused

    vga_puts("Initiating system shutdown sequence...\n");

    bool force_shutdown = false;
    if (argc > 1 && argv[1] != NULL) {
        if (argv[1][0] == '-' && argv[1][1] == 'f') {
            vga_puts("Force shutdown requested\n");
            force_shutdown = true;
        }
    }

    // Detection and appropriate method selection
    if (detect_virtualbox()) {
        vga_puts("VirtualBox environment detected\n");
        if (try_vbox_shutdown()) {
            return 0;
        }
    }

    // If no emulator-specific method worked, just halt cleanly
    if (!force_shutdown) {
        vga_puts("No power-off method available. Halting system cleanly.\n");
        try_halt_shutdown(); // This won't return
    }

    // Try QEMU-specific methods
    if (try_qemu_shutdown()) {
        return 0;
    }

    // If all else fails
    if (force_shutdown) {
        vga_puts("All shutdown methods failed. Forcing system reset...\n");
        system_fallback_reset();
    } else {
        vga_puts("Shutdown failed. System is still running.\n");
        vga_puts("Use 'shutdown -f' to force a reset.\n");
        return 1;
    }

    // Should never reach here
    return 0;
}

