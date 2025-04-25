; Advanced i386 CPU Control Assembly
; Includes: CPUID, FPU, MMX, SSE, SMP, and advanced features

[BITS 32]
SECTION .text

; Exported symbols
global cpu_detect_features
global cpu_init_fpu
global cpu_init_sse
global cpu_get_cr0
global cpu_set_cr0
global cpu_get_cr3
global cpu_set_cr3
global cpu_get_cr4
global cpu_set_cr4
global cpu_invlpg
global cpu_wbinvd
global cpu_enable_paging
global cpu_disable_paging
global cpu_enable_sse
global cpu_enable_smp
global cpu_hlt
global cpu_cli
global cpu_sti
global cpu_get_flags

; CPU Feature Flags (matches CPUID results)
struc CPU_FEATURES
    .fpu        resb 1    ; FPU present
    .vme        resb 1    ; Virtual 8086 mode extensions
    .de         resb 1    ; Debugging extensions
    .pse        resb 1    ; Page size extension
    .tsc        resb 1    ; Time stamp counter
    .msr        resb 1    ; Model-specific registers
    .pae        resb 1    ; Physical address extension
    .mce        resb 1    ; Machine check exception
    .cx8        resb 1    ; CMPXCHG8B instruction
    .apic       resb 1    ; APIC on-chip
    .sep        resb 1    ; SYSENTER/SYSEXIT
    .mtrr       resb 1    ; Memory type range registers
    .pge        resb 1    ; Page global enable
    .mca        resb 1    ; Machine check architecture
    .cmov       resb 1    ; Conditional move instructions
    .pat        resb 1    ; Page attribute table
    .pse36      resb 1    ; 36-bit page size extension
    .psn        resb 1    ; Processor serial number
    .clfsh      resb 1    ; CLFLUSH instruction
    .ds         resb 1    ; Debug store
    .acpi       resb 1    ; Thermal monitor and clock control
    .mmx        resb 1    ; MMX technology
    .fxsr       resb 1    ; FXSAVE/FXRSTOR
    .sse        resb 1    ; SSE extensions
    .sse2       resb 1    ; SSE2 extensions
    .ss         resb 1    ; Self-snoop
    .htt        resb 1    ; Hyper-threading technology
    .tm         resb 1    ; Thermal monitor
    .ia64       resb 1    ; IA-64 processor
    .pbe        resb 1    ; Pending break enable
    .sse3       resb 1    ; SSE3 extensions
    .pclmul     resb 1    ; PCLMULQDQ support
    .dtes64     resb 1    ; 64-bit debug store
    .monitor    resb 1    ; MONITOR/MWAIT
    .ds_cpl     resb 1    ; CPL qualified debug store
    .vmx        resb 1    ; Virtual machine extensions
    .smx        resb 1    ; Safer mode extensions
    .est        resb 1    ; Enhanced SpeedStep
    .tm2        resb 1    ; Thermal Monitor 2
    .ssse3      resb 1    ; Supplemental SSE3
    .cid        resb 1    ; L1 context ID
    .sdbg       resb 1    ; Silicon debug interface
    .fma        resb 1    ; Fused multiply-add
    .cx16       resb 1    ; CMPXCHG16B
    .xtpr       resb 1    ; Send Task Priority Messages
    .pdcm       resb 1    ; Performance Capabilities MSR
    .pcid       resb 1    ; Process Context Identifiers
    .dca        resb 1    ; Direct Cache Access
    .sse4_1     resb 1    ; SSE4.1
    .sse4_2     resb 1    ; SSE4.2
    .x2apic     resb 1    ; x2APIC support
    .movbe      resb 1    ; MOVBE instruction
    .popcnt     resb 1    ; POPCNT instruction
    .tsc_deadline resb 1  ; TSC deadline timer
    .aes        resb 1    ; AES instructions
    .xsave      resb 1    ; XSAVE/XRSTOR
    .osxsave    resb 1    ; OS-enabled XSAVE
    .avx        resb 1    ; Advanced Vector Extensions
    .f16c       resb 1    ; Half-precision convert
    .rdrand     resb 1    ; RDRAND instruction
    .hypervisor resb 1    ; Running under hypervisor
endstruc

; Detect CPU features and populate CPU_FEATURES structure
; void cpu_detect_features(CPU_FEATURES* features);
cpu_detect_features:
    push ebp
    mov ebp, esp
    push ebx
    push ecx
    push edx
    push edi
    
    mov edi, [ebp+8]  ; Get features struct pointer
    
    ; Check if CPUID is supported
    pushfd
    pop eax
    mov ecx, eax
    xor eax, 0x200000 ; Flip ID bit
    push eax
    popfd
    pushfd
    pop eax
    xor eax, ecx
    jz .no_cpuid      ; CPUID not supported
    
    ; Get standard features (CPUID.1:EDX)
    xor eax, eax
    inc eax
    cpuid
    
    ; EDX features
    mov byte [edi + CPU_FEATURES.fpu], 1    ; Assume FPU exists if CPUID exists
    test edx, 1 << 0  ; FPU
    jnz .fpu_ok
    mov byte [edi + CPU_FEATURES.fpu], 0
.fpu_ok:
    mov byte [edi + CPU_FEATURES.vme], dl
    shr edx, 1
    mov byte [edi + CPU_FEATURES.de], dl
    shr edx, 1
    mov byte [edi + CPU_FEATURES.pse], dl
    shr edx, 1
    mov byte [edi + CPU_FEATURES.tsc], dl
    shr edx, 1
    mov byte [edi + CPU_FEATURES.msr], dl
    shr edx, 1
    mov byte [edi + CPU_FEATURES.pae], dl
    shr edx, 1
    mov byte [edi + CPU_FEATURES.mce], dl
    shr edx, 1
    mov byte [edi + CPU_FEATURES.cx8], dl
    shr edx, 1
    mov byte [edi + CPU_FEATURES.apic], dl
    shr edx, 1
    mov byte [edi + CPU_FEATURES.sep], dl
    shr edx, 1
    mov byte [edi + CPU_FEATURES.mtrr], dl
    shr edx, 1
    mov byte [edi + CPU_FEATURES.pge], dl
    shr edx, 1
    mov byte [edi + CPU_FEATURES.mca], dl
    shr edx, 1
    mov byte [edi + CPU_FEATURES.cmov], dl
    shr edx, 1
    mov byte [edi + CPU_FEATURES.pat], dl
    shr edx, 1
    mov byte [edi + CPU_FEATURES.pse36], dl
    shr edx, 1
    mov byte [edi + CPU_FEATURES.psn], dl
    shr edx, 1
    mov byte [edi + CPU_FEATURES.clfsh], dl
    shr edx, 1
    mov byte [edi + CPU_FEATURES.ds], dl
    shr edx, 1
    mov byte [edi + CPU_FEATURES.acpi], dl
    shr edx, 1
    mov byte [edi + CPU_FEATURES.mmx], dl
    shr edx, 1
    mov byte [edi + CPU_FEATURES.fxsr], dl
    shr edx, 1
    mov byte [edi + CPU_FEATURES.sse], dl
    shr edx, 1
    mov byte [edi + CPU_FEATURES.sse2], dl
    shr edx, 1
    mov byte [edi + CPU_FEATURES.ss], dl
    shr edx, 1
    mov byte [edi + CPU_FEATURES.htt], dl
    shr edx, 1
    mov byte [edi + CPU_FEATURES.tm], dl
    shr edx, 1
    mov byte [edi + CPU_FEATURES.ia64], dl
    shr edx, 1
    mov byte [edi + CPU_FEATURES.pbe], dl
    
    ; ECX features (CPUID.1:ECX)
    mov byte [edi + CPU_FEATURES.sse3], cl
    shr ecx, 1
    mov byte [edi + CPU_FEATURES.pclmul], cl
    shr ecx, 1
    mov byte [edi + CPU_FEATURES.dtes64], cl
    shr ecx, 1
    mov byte [edi + CPU_FEATURES.monitor], cl
    shr ecx, 1
    mov byte [edi + CPU_FEATURES.ds_cpl], cl
    shr ecx, 1
    mov byte [edi + CPU_FEATURES.vmx], cl
    shr ecx, 1
    mov byte [edi + CPU_FEATURES.smx], cl
    shr ecx, 1
    mov byte [edi + CPU_FEATURES.est], cl
    shr ecx, 1
    mov byte [edi + CPU_FEATURES.tm2], cl
    shr ecx, 1
    mov byte [edi + CPU_FEATURES.ssse3], cl
    shr ecx, 1
    mov byte [edi + CPU_FEATURES.cid], cl
    shr ecx, 1
    mov byte [edi + CPU_FEATURES.sdbg], cl
    shr ecx, 1
    mov byte [edi + CPU_FEATURES.fma], cl
    shr ecx, 1
    mov byte [edi + CPU_FEATURES.cx16], cl
    shr ecx, 1
    mov byte [edi + CPU_FEATURES.xtpr], cl
    shr ecx, 1
    mov byte [edi + CPU_FEATURES.pdcm], cl
    shr ecx, 1
    mov byte [edi + CPU_FEATURES.pcid], cl
    shr ecx, 1
    mov byte [edi + CPU_FEATURES.dca], cl
    shr ecx, 1
    mov byte [edi + CPU_FEATURES.sse4_1], cl
    shr ecx, 1
    mov byte [edi + CPU_FEATURES.sse4_2], cl
    shr ecx, 1
    mov byte [edi + CPU_FEATURES.x2apic], cl
    shr ecx, 1
    mov byte [edi + CPU_FEATURES.movbe], cl
    shr ecx, 1
    mov byte [edi + CPU_FEATURES.popcnt], cl
    shr ecx, 1
    mov byte [edi + CPU_FEATURES.tsc_deadline], cl
    shr ecx, 1
    mov byte [edi + CPU_FEATURES.aes], cl
    shr ecx, 1
    mov byte [edi + CPU_FEATURES.xsave], cl
    shr ecx, 1
    mov byte [edi + CPU_FEATURES.osxsave], cl
    shr ecx, 1
    mov byte [edi + CPU_FEATURES.avx], cl
    shr ecx, 1
    mov byte [edi + CPU_FEATURES.f16c], cl
    shr ecx, 1
    mov byte [edi + CPU_FEATURES.rdrand], cl
    shr ecx, 1
    
    ; Check for hypervisor
    mov eax, 0x1
    cpuid
    test ecx, 1 << 31
    setnz byte [edi + CPU_FEATURES.hypervisor]
    
    ; Get extended features (CPUID.80000001h:EDX/ECX)
    mov eax, 0x80000001
    cpuid
    
    ; TODO: Process extended features
    
.no_cpuid:
    pop edi
    pop edx
    pop ecx
    pop ebx
    pop ebp
    ret

; Initialize FPU
; void cpu_init_fpu(void);
cpu_init_fpu:
    fninit
    ret

; Initialize SSE (must have checked for support first)
; void cpu_init_sse(void);
cpu_init_sse:
    mov eax, cr0
    and ax, 0xFFFB      ; Clear coprocessor emulation CR0.EM
    or ax, 0x2          ; Set coprocessor monitoring CR0.MP
    mov cr0, eax
    
    mov eax, cr4
    or ax, 3 << 9        ; Set CR4.OSFXSR and CR4.OSXMMEXCPT
    mov cr4, eax
    
    ret

; Enable SSE extensions
; void cpu_enable_sse(void);
cpu_enable_sse:
    push eax
    mov eax, cr0
    and eax, ~(1 << 2)  ; Clear CR0.EM (FPU emulation)
    or eax, (1 << 1)    ; Set CR0.MP (Monitor FPU)
    mov cr0, eax
    
    mov eax, cr4
    or eax, (1 << 9)    ; Set CR4.OSFXSR (SSE enabled)
    or eax, (1 << 10)   ; Set CR4.OSXMMEXCPT (SSE exceptions)
    mov cr4, eax
    
    pop eax
    ret

; Get CR0 register
; uint32_t cpu_get_cr0(void);
cpu_get_cr0:
    mov eax, cr0
    ret

; Set CR0 register
; void cpu_set_cr0(uint32_t value);
cpu_set_cr0:
    mov eax, [esp+4]
    mov cr0, eax
    ret

; Get CR3 register (page directory base)
; uint32_t cpu_get_cr3(void);
cpu_get_cr3:
    mov eax, cr3
    ret

; Set CR3 register (page directory base)
; void cpu_set_cr3(uint32_t value);
cpu_set_cr3:
    mov eax, [esp+4]
    mov cr3, eax
    ret

; Get CR4 register
; uint32_t cpu_get_cr4(void);
cpu_get_cr4:
    mov eax, cr4
    ret

; Set CR4 register
; void cpu_set_cr4(uint32_t value);
cpu_set_cr4:
    mov eax, [esp+4]
    mov cr4, eax
    ret

; Invalidate TLB entry for linear address
; void cpu_invlpg(void* linear_addr);
cpu_invlpg:
    mov eax, [esp+4]
    invlpg [eax]
    ret

; Write back and invalidate cache
; void cpu_wbinvd(void);
cpu_wbinvd:
    wbinvd
    ret

; Enable paging
; void cpu_enable_paging(void);
cpu_enable_paging:
    mov eax, cr0
    or eax, 0x80000000  ; Set CR0.PG
    mov cr0, eax
    ret

; Disable paging
; void cpu_disable_paging(void);
cpu_disable_paging:
    mov eax, cr0
    and eax, ~0x80000000 ; Clear CR0.PG
    mov cr0, eax
    ret

; Enable SMP features (must be called on BSP only)
; void cpu_enable_smp(void);
cpu_enable_smp:
    ; Enable APIC (if available)
    mov ecx, 0x1B       ; IA32_APIC_BASE MSR
    rdmsr
    or eax, 0x800       ; Set APIC enable bit
    wrmsr
    
    ; TODO: More SMP initialization
    ret

; Halt the CPU
; void cpu_hlt(void);
cpu_hlt:
    hlt
    ret

; Disable interrupts
; void cpu_cli(void);
cpu_cli:
    cli
    ret

; Enable interrupts
; void cpu_sti(void);
cpu_sti:
    sti
    ret

; Get EFLAGS register
; uint32_t cpu_get_flags(void);
cpu_get_flags:
    pushfd
    pop eax
    ret
