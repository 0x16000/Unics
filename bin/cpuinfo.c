#include <stdio.h>
#include <arch/i386/cpu.h>

int cpuinfo_main(int argc, char **argv) {
    (void)argc; (void)argv; // Suppress unused parameter warnings

    cpu_features_t features;
    cpu_detect_features(&features);

    printf("\nCPU Information:\n");
    printf("=================\n");

    // Basic Features
    printf("-- Core Features --\n");
    printf("FPU:         %s\n", features.fpu ? "Yes" : "No");
    printf("VME:         %s\n", features.vme ? "Yes" : "No");
    printf("DE:          %s\n", features.de ? "Yes" : "No");
    printf("PSE:         %s\n", features.pse ? "Yes" : "No");
    printf("TSC:         %s\n", features.tsc ? "Yes" : "No");
    printf("MSR:         %s\n", features.msr ? "Yes" : "No");
    printf("MCA:         %s\n", features.mca ? "Yes" : "No");
    printf("CMPXCHG8B:   %s\n", features.cx8 ? "Yes" : "No");
    printf("APIC:        %s\n", features.apic ? "Yes" : "No");
    
    // Memory Management
    printf("\n-- Memory Management --\n");
    printf("PAE:         %s\n", features.pae ? "Yes" : "No");
    printf("PGE:         %s\n", features.pge ? "Yes" : "No");
    printf("MTRR:        %s\n", features.mtrr ? "Yes" : "No");
    printf("PAT:         %s\n", features.pat ? "Yes" : "No");
    printf("PSE-36:      %s\n", features.pse36 ? "Yes" : "No");

    // SIMD Extensions
    printf("\n-- SIMD Extensions --\n");
    printf("MMX:         %s\n", features.mmx ? "Yes" : "No");
    printf("SSE:         %s\n", features.sse ? "Yes" : "No");
    printf("SSE2:        %s\n", features.sse2 ? "Yes" : "No");
    printf("SSE3:        %s\n", features.sse3 ? "Yes" : "No");
    printf("SSSE3:       %s\n", features.ssse3 ? "Yes" : "No");
    printf("SSE4.1:      %s\n", features.sse4_1 ? "Yes" : "No");
    printf("SSE4.2:      %s\n", features.sse4_2 ? "Yes" : "No");
    printf("AVX:         %s\n", features.avx ? "Yes" : "No");
    printf("FMA:         %s\n", features.fma ? "Yes" : "No");

    // Virtualization & Security
    printf("\n-- Advanced Features --\n");
    printf("VMX:         %s\n", features.vmx ? "Yes" : "No");
    printf("SMX:         %s\n", features.smx ? "Yes" : "No");
    printf("AES-NI:      %s\n", features.aes ? "Yes" : "No");
    printf("RDRAND:      %s\n", features.rdrand ? "Yes" : "No");
    
    // Power Management
    printf("\n-- Power Management --\n");
    printf("ACPI:        %s\n", features.acpi ? "Yes" : "No");
    printf("EST:         %s\n", features.est ? "Yes" : "No");
    printf("TM:          %s\n", features.tm ? "Yes" : "No");
    printf("TM2:         %s\n", features.tm2 ? "Yes" : "No");

    // Other Features
    printf("\n-- Other Features --\n");
    printf("Hypervisor:  %s\n", features.hypervisor ? "Yes" : "No");
    printf("HTT:         %s\n", features.htt ? "Yes" : "No");
    printf("POPCNT:      %s\n", features.popcnt ? "Yes" : "No");
    printf("XSAVE:       %s\n", features.xsave ? "Yes" : "No");
    
    return 0;
}
