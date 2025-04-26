#include <stdio.h>
#include <vers.h>
#include <arch/i386/cpu.h>

static void print_cpu_brand() {
    char brand[49] = {0};
    
    // Check if extended CPUID is supported
    unsigned int max_extended;
    asm volatile("cpuid" : "=a"(max_extended) : "a"(0x80000000) : "ebx", "ecx", "edx");
    
    if (max_extended >= 0x80000004) {
        unsigned int *brand_ptr = (unsigned int*)brand;
        asm volatile(
            "cpuid\n\t"
            : "=a"(brand_ptr[0]), "=b"(brand_ptr[1]), "=c"(brand_ptr[2]), "=d"(brand_ptr[3]) 
            : "a"(0x80000002)
        );
        asm volatile(
            "cpuid"
            : "=a"(brand_ptr[4]), "=b"(brand_ptr[5]), "=c"(brand_ptr[6]), "=d"(brand_ptr[7]) 
            : "a"(0x80000003)
        );
        asm volatile(
            "cpuid"
            : "=a"(brand_ptr[8]), "=b"(brand_ptr[9]), "=c"(brand_ptr[10]), "=d"(brand_ptr[11]) 
            : "a"(0x80000004)
        );
        
        // Clean up the brand string
        char *p = brand;
        while (*p == ' ') p++;  // Skip leading spaces
        printf("%.48s", p);     // Print max 48 chars of cleaned string
    } else {
        printf("Unknown CPU");
    }
}

int fetch_main(int argc, char **argv) {
    printf("    ,        ,        /unics$\n");
    printf("   /(        )`       -------\n");
    printf("   \\ \\___   / |       Kernel: ");
    printf(OS_NAME);
    printf("\n");
    printf("   /- _  `-/  '       Shell: sh\n");
    printf("  (/\\/ \\ \\   /\\       CPU: ");
    print_cpu_brand();
    printf("\n");
    printf("  / /   | `    \\\n");
    printf("  O O   ) /    |\n");
    printf("  `-^--'`<     '\n");
    printf(" (_.)  _  )   /\n");
    printf("  `.___/`    /\n");
    printf("    `-----' /\n");
    printf("          `\n");

    return 0;
}
