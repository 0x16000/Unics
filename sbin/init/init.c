#include <vga.h>
#include <sys/multiboot.h>
#include <shell.h>
#include <keyboard.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <login.h>
#include <arch/i386/cpu.h>
#include <time.h>
#include <sys/fs.h>
#include <sys/process.h>
#include <vmm.h>
#include <pmm.h>
#include <sha2.h>
#include <paging.h>
#include <sys/null.h>
#include <sys/refcnt.h>
#include <sys/srp.h>
#include <hdmi.h>

#define SHORT_DELAY  50000
#define MEDIUM_DELAY 100000
#define LONG_DELAY   150000

extern shell_command_t shell_commands[];
extern size_t shell_commands_count;

static const struct multiboot_header multiboot_header __attribute__((used)) = {
    .magic = MULTIBOOT_HEADER_MAGIC,
    .flags = MULTIBOOT_HEADER_FLAGS,
    .checksum = -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
};

extern uint32_t multiboot_info_ptr;
extern uint32_t __bitmap_start;

struct refcnt ss_refcnt;
static struct srp ss_srp;

void early_cpu_init(void);

int main(void) {
    // Initialize basic I/O
    vga_initialize();
    vga_disable_cursor();
    vga_clear();
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);

    // Boot banner
    vga_printf("Unics/i686 0.1-RELEASE #0: %s\n", __DATE__);
    vga_puts("Copyright (c) 2025 0x16000. All rights reserved.\n\n");
    delay(MEDIUM_DELAY);

    // Hardware identification
    vga_puts("CPU: i686-class processor\n");
    delay(SHORT_DELAY);

    // Display setup
    vga_puts("\nInitializing display controllers:\n");
    vga_puts("vga0: <Generic VGA> at port 0x3c0-0x3df iomem 0xa0000-0xbffff\n");
    delay(SHORT_DELAY);

    vga_puts("hdmi0: <Broadcom HDMI Controller> at iomem 0xfe902000\n");
    hdmi_device_t hdmi;
    if (hdmi_init(&hdmi, HDMI_BASE_ADDR) == 0) {
        hdmi_set_resolution(&hdmi, HDMI_RES_1920x1080_60Hz);
        hdmi_set_color_depth(&hdmi, HDMI_COLOR_DEPTH_8BIT);
        hdmi_enable(&hdmi, true);
        hdmi_enable_video(&hdmi, true);
        vga_puts(hdmi_is_connected(&hdmi)
                 ? "hdmi0: connected (1920x1080@60Hz)\n"
                 : "hdmi0: no display connected\n");
    } else {
        vga_puts("hdmi0: initialization failed (fallback to VGA)\n");
    }
    delay(MEDIUM_DELAY);
    vga_puts("\n");

    // Memory management
    paging_init();
    vga_puts("Paging: initialization complete\n\n");
    delay(SHORT_DELAY);

    // Device messages
    vga_puts("Probing devices:\n");
    delay(SHORT_DELAY);
    vga_puts("kbd0 at atkbdc0 (kbd port)\n");
    delay(SHORT_DELAY);
    vga_puts("sc0: <System console> at flags 0x100 on isa0\n\n");
    delay(SHORT_DELAY);

    // CPU & hardware initialization
    early_cpu_init();

    // Filesystem setup
    vga_puts("Mounting root from RAM\n");
    delay(SHORT_DELAY);

    fs_init();
    fs_mkdir("root");
    fs_create("README");
    fs_open(&root_fs, "README");
    const char *text =
        "Welcome to Unics and thank you for choosing it!\n"
        "First edition Unics is the first version of this kernel / operating system.\n"
        "As said before, for a list of commands run the 'help' command.\n"
        "Thank you, have a nice day.\n";
    fs_write(&root_fs, "README", text, strlen(text));
    fs_close(&root_fs, "README");
    delay(SHORT_DELAY);

    // Memory & resource managers
    refcnt_init(&ss_refcnt);
    srp_init(&ss_srp);
    vmm_init();
    vga_puts("vmm: Virtual memory initialized\n");
    delay(SHORT_DELAY);

    pmm_init();
    pmm_add_region(0x00100000, 0x07F00000, PMM_ZONE_NORMAL);
    pmm_add_region(0x00100000, 0x00F00000, PMM_ZONE_DMA);
    pmm_reserve_range(0x00000000, 0x00100000);
    pmm_reserve_range(0x00100000, 0x00200000);
    vga_puts("pmm: Physical memory initialized\n");
    delay(SHORT_DELAY);

    null_init();
    vga_puts("null: dev/null/ initialized\n");
    delay(SHORT_DELAY);

    // Process subsystem
    process_init();
    process_create("init", 0);
    process_create("shell", 1);

    // Keyboard driver
    kb_init();
    kb_enable_input(true);
    kb_set_boot_complete(true);

    // Final system messages
    vga_puts("Kernel initialization complete.\n\n");
    delay(MEDIUM_DELAY);

    vga_puts("Starting system services...\n");
    delay(LONG_DELAY);
    vga_puts("Starting init process\n\n");
    delay(MEDIUM_DELAY);
    vga_enable_cursor();

    // Login and welcome
    login_prompt();

    vga_puts("\nUnics/i686 0.1-RELEASE (GENERIC)\n\n");
    vga_puts("Welcome to Unics!\n\n");
    vga_puts("This software is provided \"as is\" without any express or\n");
    vga_puts("implied warranties, including, without limitation, the implied\n");
    vga_puts("warranties of merchantability and fitness for a particular purpose.\n\n");
    vga_puts("Report bugs on GitHub by opening an issue or pull request.\n\n");

    // Launch shell
    shell_context_t shell_ctx;
    shell_init(&shell_ctx, shell_commands, shell_commands_count);
    shell_run(&shell_ctx);

    return 0;
}

void early_cpu_init(void) {
    cpu_features_t features;
    cpu_detect_features(&features);

    vga_puts("cpu0: features=0x1bf <FPU,VME,DE,PSE,TSC,MSR,PAE,MCE>\n");
    delay(MEDIUM_DELAY);

    cpu_init_fpu();
    vga_puts("cpu0: FPU initialized\n");
    delay(SHORT_DELAY);

    if (features.sse) {
        cpu_init_sse();
        vga_puts("cpu0: SSE extensions enabled\n");
        delay(SHORT_DELAY);
    }

    if (features.pae) {
        cpu_set_cr4(cpu_get_cr4() | CR4_PAE);
        vga_puts("cpu0: PAE enabled\n");
        delay(SHORT_DELAY);
    }

    if (features.sse || features.sse2) {
        cpu_enable_sse();
    }

    if (features.apic) {
        cpu_enable_smp();
        vga_puts("cpu0: APIC initialized\n");
        delay(SHORT_DELAY);
    }
}
