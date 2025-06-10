#include <shell.h>
#include <vga.h>
#include <stdint.h>
#include <io.h>

/**
 * Attempts a VirtualBox reboot via the 0x4004 port.
 */
static bool try_vbox_reboot() {
    vga_puts("Attempting VirtualBox reboot...\n");

    // Write reboot command to VirtualBox power management port (0x4004)
    __asm__ volatile (
        "outw %w0, %w1"
        :
        : "a" ((uint16_t)0x2000), "Nd" ((uint16_t)0x4004)  // Fixed: Changed from 0x3400 to 0x2000
        : "memory"
    );

    // Wait to see if reboot occurs
    for (volatile int i = 0; i < 1000000; i++) {  // Made counter volatile
        io_wait();
    }
    return false;
}

/**
 * Try to reboot using the 8042 keyboard controller reset command.
 */
static bool try_8042_reset(void) {
    vga_puts("Attempting 8042 controller reset...\n");

    uint8_t status;
    int timeout = 500000;

    // Disable interrupts
    __asm__ volatile ("cli");

    // Wait for 8042 controller to be ready
    do {
        __asm__ volatile ("inb $0x64, %0" : "=a"(status));
        io_wait();
        if (--timeout == 0) {
            vga_puts("8042 controller timeout.\n");
            __asm__ volatile ("sti");  // Re-enable interrupts before returning
            return false;
        }
    } while (status & 0x02);

    // Send reset command
    __asm__ volatile ("outb %0, $0x64" : : "a"((uint8_t)0xFE));
    io_wait();

    vga_puts("Reset command sent to 8042 controller.\n");

    // Wait for reset
    for (volatile int i = 0; i < 1000000; i++) {  // Made counter volatile
        io_wait();
    }

    __asm__ volatile ("sti");  // Re-enable interrupts if we're still here
    return false;  // If we reach here, reset failed
}

/**
 * Try PCI reset method
 */
static bool try_pci_reset(void) {
    vga_puts("Attempting PCI reset...\n");
    
    // Try to reset via PCI configuration space
    // This is a simplified approach - real implementation would need PCI enumeration
    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"((uint8_t)0x06), "Nd"((uint16_t)0xCF9)  // Reset CPU + System Reset
        : "memory"
    );
    
    for (volatile int i = 0; i < 100000; i++) {
        io_wait();
    }
    
    return false;
}

/**
 * Force a system reset by triggering a triple fault.
 */
static void system_fallback_reset(void) {
    vga_puts("Attempting triple fault reset...\n");

    // Disable interrupts
    __asm__ volatile ("cli");

    // Create proper null IDT descriptor
    struct {
        uint16_t limit;
        uint32_t base;
    } __attribute__((packed)) null_idt = {0, 0};

    // Load the null IDT
    __asm__ volatile ("lidt %0" : : "m" (null_idt) : "memory");

    // Trigger a triple fault
    __asm__ volatile ("int $0x03");

    // Alternative method
    volatile int zero = 0;
    volatile int result = 1 / zero;  // This should cause exception
    (void)result;  // Prevent compiler warning

    // If we get here, just halt
    for(;;) {
        __asm__ volatile ("hlt");
    }
}

/**
 * Main entry point for the reboot command.
 */
int reboot_main(int argc, char **argv) {
    (void)argv;

    vga_puts("Initiating system reboot...\n");

    // Check for command-line options
    if (argc > 1 && argv[1] != NULL && argv[1][0] == '-') {  // Added null check
        if (argv[1][1] == 'f') {  // Check for -f specifically
            vga_puts("Forced reboot option detected...\n");
        }
    }

    // Try VirtualBox reboot first
    if (try_vbox_reboot()) {
        return 0;
    }

    // Try PCI reset
    if (try_pci_reset()) {
        return 0;
    }

    // Try 8042 controller reset
    if (try_8042_reset()) {
        return 0;
    }

    // Fallback to triple fault
    vga_puts("Primary reset methods failed, trying fallback...\n");
    system_fallback_reset();

    // Should never reach here
    vga_puts("All reboot methods failed! System halted.\n");
    for(;;) {
        __asm__ volatile ("hlt");
    }
}
