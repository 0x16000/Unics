; Multiboot header constants
MB_MAGIC     equ 0x1BADB002
MB_FLAGS     equ 0x00000003    ; Align modules on page boundaries + provide memory map
MB_CHECKSUM  equ -(MB_MAGIC + MB_FLAGS)

section .multiboot
align 4
    dd MB_MAGIC                ; Magic number (required)
    dd MB_FLAGS                ; Flags
    dd MB_CHECKSUM             ; Checksum (magic + flags + checksum = 0)

section .text
global _start
extern main             ; Changed from 'main' to more descriptive 'kernel_main'
global multiboot_info_ptr      ; Export multiboot info pointer to kernel

_start:
    ; Set up stack (16KB aligned to 16 bytes)
    mov esp, stack_top
    
    ; Save multiboot info pointer (passed by bootloader in EBX)
    mov [multiboot_info_ptr], ebx
    
    ; Clear direction flag (std functions expect forward movement)
    cld
    
    ; Optional: Verify CPU features here (FPU, SSE, etc.)
    
    ; Call kernel main function
    call main
    
    ; If kernel_main returns (shouldn't happen), halt the system
    cli                         ; Disable interrupts
.hang:
    hlt                         ; Halt CPU
    jmp .hang                   ; In case of NMI

section .bss
align 16                       ; Align stack to 16 bytes for performance
stack_bottom:
    resb 16384                 ; 16KB stack
stack_top:

; Store multiboot info pointer (32-bit on x86)
multiboot_info_ptr: 
    resd 1                     ; Reserve 4 bytes

section .note.GNU-stack noalloc noexec nowrite progbits ; Mark stack as non-executable