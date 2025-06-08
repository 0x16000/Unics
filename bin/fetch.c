#include <stdio.h>
#include <vers.h>
#include <arch/i386/cpu.h>

// Print the CPU brand string using extended CPUID calls
static void print_cpu_brand() {
    // 48 bytes + null terminator
    char brand[49] = {0};
    
    unsigned int max_extended;
    __asm__ volatile(
        "cpuid" 
        : "=a"(max_extended) 
        : "a"(0x80000000) 
        : "ebx", "ecx", "edx"
    );
    
    if (max_extended >= 0x80000004) {
        unsigned int *brand_ptr = (unsigned int*)brand;
        __asm__ volatile(
            "cpuid\n\t"
            : "=a"(brand_ptr[0]), "=b"(brand_ptr[1]), "=c"(brand_ptr[2]), "=d"(brand_ptr[3]) 
            : "a"(0x80000002)
            : "memory"
        );
        __asm__ volatile(
            "cpuid\n\t"
            : "=a"(brand_ptr[4]), "=b"(brand_ptr[5]), "=c"(brand_ptr[6]), "=d"(brand_ptr[7]) 
            : "a"(0x80000003)
            : "memory"
        );
        __asm__ volatile(
            "cpuid\n\t"
            : "=a"(brand_ptr[8]), "=b"(brand_ptr[9]), "=c"(brand_ptr[10]), "=d"(brand_ptr[11]) 
            : "a"(0x80000004)
            : "memory"
        );
        
        brand[48] = '\0'; // Ensure null termination
        
        // Trim leading spaces
        char *p = brand;
        while (*p == ' ') p++;
        printf("%.48s", p);
    } else {
        printf("Unknown CPU");
    }
}

int fetch_main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    printf("\n");
    printf("                        `          root@unics\n");
    printf("  `.`....---.......--.```   -/     ------------\n");
    printf("  +o   .--`         /y:`      +.   OS: " OS_NAME "\n");
    printf("   yo`:.            :o      `+-    Host: root\n");
    printf("    y/               -/`   -o/     Kernel: " OS_VERSION "\n");
    printf("   .-                  ::/sy+:.    resolution: 640x400\n");
    printf("   /                     `--  /    fs: unics_fs\n");
    printf("  `:                          :`   Shell: sh\n");
    printf("  `:                          :`   CPU: ");
    print_cpu_brand();
    printf("\n");
    printf("   /                          /    \n");
    printf("   .-                        -.    \n");
    printf("    --                      -.     \n");
    printf("     `:`                  `:`      \n");
    printf("       .--             `--.        \n");
    printf("          .---.....----.           \n");
    printf("                                   \n");

    return 0;
}
