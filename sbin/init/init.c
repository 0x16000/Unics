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
#include <sys/process.h>
#include <vmm.h>
#include <pmm.h>

extern shell_command_t shell_commands[];
extern size_t shell_commands_count;

static const struct multiboot_header multiboot_header __attribute__((used)) = {
    .magic = MULTIBOOT_HEADER_MAGIC,
    .flags = MULTIBOOT_HEADER_FLAGS,
    .checksum = -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
};

extern uint32_t multiboot_info_ptr;
extern uint32_t __bitmap_start;

void early_cpu_init(void);

int main(void) {
    // Initialize VGA text mode
    vga_initialize();
    vga_disable_cursor();
    vga_clear();
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);

    // BSD-style boot header
    vga_puts("Unics/i386 0.1-RELEASE #0: ");
    vga_puts(__DATE__);
    vga_puts("\n");
    vga_puts("Copyright (c) 2025 0x16000. All rights reserved.\n");
    vga_puts("\n");
    
    delay(100000);
    
    // System identification
    vga_puts("CPU: i386-class processor\n");
    delay(50000);
    
    vga_puts("\n");
    
    // Device probing messages
    vga_puts("Probing devices:\n");
    delay(100000);
    
    vga_puts("kbd0 at atkbdc0 (kbd port)\n");
    delay(100000);
    
    vga_puts("vga0: <Generic VGA> at port 0x3c0-0x3df iomem 0xa0000-0xbffff\n");
    delay(100000);
    
    vga_puts("sc0: <System console> at flags 0x100 on isa0\n");
    delay(50000);
    
    vga_puts("\n");
    
    // Initialize CPU features
    early_cpu_init();
    
    // System initialization messages
    vga_puts("Mounting root from RAM\n");
    delay(100000);
    
    // Initialize Filesystem
    fs_init(); // initialize the global root_fs and current_dir

    // Use &root_fs as the fs pointer for file operations
    fs_mkdir("root"); // create directory "root"
    fs_create("README"); // create file "README"
    fs_open(&root_fs, "README"); // open README for writing

    const char *text = "Welcome to Unics and thank you for choosing it!\n"
    "First edition Unics is the first version of this kernel / operating system.\n"
    "As said before for a list of commands run the 'help' command.\n"
    "Thank you, have a nice day.\n";
    fs_write(&root_fs, "README", text, strlen(text)); // write to README

    fs_close(&root_fs, "README"); // close file when done

    delay(100000);

    // Initialize virtual memory
    vmm_init();
    vga_puts("vmm: Virtual memory initialized\n");
    delay(100000);

    // Initialize physical memory manager
    pmm_init();
    vga_puts("pmm: Physical memory initialized\n");
    delay(50000);

    // Create processes
    process_init();
    process_create("init", 0);
    process_create("shell", 1);
    
    // Initialize keyboard
    kb_init();
    kb_enable_input(true);
    kb_set_boot_complete(true);
    
    vga_puts("Kernel initialization complete.\n");
    delay(100000);
    
    vga_puts("\n");
    vga_puts("Starting system services...\n");
    delay(200000);
    
    vga_puts("Starting init process\n");
    delay(100000);
    
    vga_puts("\n");
    vga_enable_cursor();
    
    // Login prompt
    login_prompt();
    
    // BSD-style welcome message
    vga_puts("\n");
    vga_puts("Unics/i386 0.1-RELEASE (GENERIC)\n");
    vga_puts("\n");
    vga_puts("Welcome to Unics!\n");
    vga_puts("\n");
    vga_puts("This software is provided \"as is\" and without any express or\n");
    vga_puts("implied warranties, including, without limitation, the implied\n");
    vga_puts("warranties of merchantability and fitness for a particular purpose.\n");
    vga_puts("\n");
    vga_puts("Report bugs to the github using/opening a issue or pull reqeust.\n");
    vga_puts("\n");
    
    // Start shell
    shell_context_t shell_ctx;
    shell_init(&shell_ctx, shell_commands, shell_commands_count);
    shell_run(&shell_ctx);
    
    return 0;
}

void early_cpu_init(void) {
    cpu_features_t features;
    cpu_detect_features(&features);
    
    vga_puts("cpu0: features=0x");
    // Print hex representation of features (you'll need to implement this)
    vga_puts("1bf<FPU,VME,DE,PSE,TSC,MSR,PAE,MCE>\n");
    delay(100000);
    
    cpu_init_fpu();
    vga_puts("cpu0: FPU initialized\n");
    delay(50000);
    
    if (features.sse) {
        cpu_init_sse();
        vga_puts("cpu0: SSE extensions enabled\n");
        delay(50000);
    }
    
    if (features.pae) {
        uint32_t cr4 = cpu_get_cr4();
        cpu_set_cr4(cr4 | CR4_PAE);
        vga_puts("cpu0: PAE enabled\n");
        delay(50000);
    }
    
    if (features.sse || features.sse2) {
        cpu_enable_sse();
    }
    
    if (features.apic) {
        cpu_enable_smp();
        vga_puts("cpu0: APIC initialized\n");
        delay(50000);
    }
}