#include <vga.h>
#include <sys/multiboot.h>
#include <shell.h>
#include <keyboard.h>
#include <stdint.h>
#include <stdbool.h>
#include <login.h>
#include <arch/i386/cpu.h>
#include <time.h>
#include <sys/fs.h>

extern shell_command_t shell_commands[];
extern size_t shell_commands_count;

static const struct multiboot_header multiboot_header __attribute__((used)) = {
    .magic     = MULTIBOOT_HEADER_MAGIC,
    .flags     = MULTIBOOT_HEADER_FLAGS,
    .checksum  = -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
};

extern uint32_t multiboot_info_ptr;
extern uint32_t __bitmap_start;

void early_cpu_init(void);

int main(void) {
    // Initialize VGA text mode
    vga_initialize();
    vga_disable_cursor();
    vga_clear();

    // Print header
    vga_puts(">> Unics/i386");

    // boot> prompt at (0,1)
    vga_puts_at("boot> ", 0, 1);
    delay(10000);
    vga_puts_at("entry point at 0xC0000000", 0, 2); // Default Modern Kernel Operating System hex value 32-bit

    // Kernel output text color: White on blue
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE);

    // boot info
    delay(100000);
    vga_puts_at("Firmware boot", 0, 3);
    delay(150000);
    vga_puts_at("i386 functional", 0, 4);
    delay(100000);
    vga_puts_at("FS loaded", 0, 5);
    vga_puts_at("Copyright (c) 2025 0x16000. All rights reserved.", 0, 6);

    vga_enable_cursor();
    vga_update_cursor(0, 7); // Move cursor to start of line 3

    delay(200000); // Pause for ~2 seconds to show boot info

    // Switch back to normal text color (light grey on black)
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_clear(); // Clear screen with the new color (black background)

    // Initialize CPU features
    early_cpu_init();

    // Initialize Filesystem
    fs_init();

    // Initialize keyboard
    kb_init();
    kb_enable_input(true);
    kb_set_boot_complete(true);

    // Login prompt
    login_prompt();

    // Welcome prompt
    vga_puts("Welcome to First Edition Unics.  You may be sure that it\n"
         "is suitably protected by ironclad licences, contractual agreements,\n"
         "living wills, and trade secret laws of every sort.  A trolley car is\n"
         "certain to grow in your stomach if you violate the conditions\n"
         "under which you got this tape.  Consult your lawyer in case of any doubt.\n"
         "If doubt persists, consult our lawyers.\n"
         "\n"
         "Please commit this message to memory.  If this is a hardcopy terminal,\n"
         "tear off the paper and affix it to your machine.  Otherwise\n"
         "take a photo of your screen.\n"
         "\n"
         "Thank you for choosing First Edition Unics.  Have a nice day.\n");


    // Start shell
    shell_context_t shell_ctx;
    shell_init(&shell_ctx, shell_commands, shell_commands_count);
    shell_run(&shell_ctx);

    return 0;
}

void early_cpu_init(void) {
    cpu_features_t features;
    cpu_detect_features(&features);

    cpu_init_fpu();

    if (features.sse) {
        cpu_init_sse();
    }

    if (features.pae) {
        uint32_t cr4 = cpu_get_cr4();
        cpu_set_cr4(cr4 | CR4_PAE);
    }

    if (features.sse || features.sse2) {
        cpu_enable_sse();
    }

    if (features.apic) {
        cpu_enable_smp();
    }
}
