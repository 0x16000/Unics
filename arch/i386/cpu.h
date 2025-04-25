#ifndef ARCH_I386_CPU_H
#define ARCH_I386_CPU_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    bool fpu;
    bool vme;
    bool de;
    bool pse;
    bool tsc;
    bool msr;
    bool pae;
    bool mce;
    bool cx8;
    bool apic;
    bool sep;
    bool mtrr;
    bool pge;
    bool mca;
    bool cmov;
    bool pat;
    bool pse36;
    bool psn;
    bool clfsh;
    bool ds;
    bool acpi;
    bool mmx;
    bool fxsr;
    bool sse;
    bool sse2;
    bool ss;
    bool htt;
    bool tm;
    bool ia64;
    bool pbe;
    bool sse3;
    bool pclmul;
    bool dtes64;
    bool monitor;
    bool ds_cpl;
    bool vmx;
    bool smx;
    bool est;
    bool tm2;
    bool ssse3;
    bool cid;
    bool sdbg;
    bool fma;
    bool cx16;
    bool xtpr;
    bool pdcm;
    bool pcid;
    bool dca;
    bool sse4_1;
    bool sse4_2;
    bool x2apic;
    bool movbe;
    bool popcnt;
    bool tsc_deadline;
    bool aes;
    bool xsave;
    bool osxsave;
    bool avx;
    bool f16c;
    bool rdrand;
    bool hypervisor;
} cpu_features_t;

// Function declarations
void cpu_detect_features(cpu_features_t* features);
void cpu_init_fpu(void);
void cpu_init_sse(void);
void cpu_enable_sse(void);
void cpu_enable_smp(void);
uint32_t cpu_get_cr0(void);
void cpu_set_cr0(uint32_t value);
uint32_t cpu_get_cr3(void);
void cpu_set_cr3(uint32_t value);
uint32_t cpu_get_cr4(void);
void cpu_set_cr4(uint32_t value);
void cpu_invlpg(void* linear_addr);
void cpu_wbinvd(void);
void cpu_enable_paging(void);
void cpu_disable_paging(void);
void cpu_hlt(void);
void cpu_cli(void);
void cpu_sti(void);
uint32_t cpu_get_flags(void);

// CR4 bit definitions
#define CR4_PAE         (1 << 5)
#define CR4_PSE         (1 << 4)
#define CR4_OSFXSR      (1 << 9)
#define CR4_OSXMMEXCPT  (1 << 10)

#endif // ARCH_I386_CPU_H
