; Multiboot header constants
MB_MAGIC     equ 0x1BADB002
MB_FLAGS     equ 0x00000003    ; Align modules on page boundaries + provide memory map
MB_CHECKSUM  equ -(MB_MAGIC + MB_FLAGS)

; Ensure the multiboot header is at the very start of the binary
section .multiboot
align 4
    dd MB_MAGIC                ; Magic number (required)
    dd MB_FLAGS                ; Flags
    dd MB_CHECKSUM             ; Checksum (magic + flags + checksum = 0)

section .text
global _start
extern main
global multiboot_info_ptr

_start:
    ; Set up stack (ensure 16-byte alignment)
    mov esp, stack_top
    
    ; Save multiboot info pointer (EBX is guaranteed to be valid by the bootloader)
    mov [multiboot_info_ptr], ebx
    
    ; Clear direction flag (standard for C code)
    cld
    
    ; Call kernel main
    call main
    
    ; Halt if main returns (with interrupts disabled)
    cli
.hang:
    hlt
    jmp .hang

section .bss
align 16
stack_bottom:
    resb 16384                 ; 16KB stack
stack_top:

section .data
align 4
multiboot_info_ptr: 
    dd 0                       ; Initialize to 0 (though bootloader will overwrite)

section .note.GNU-stack noalloc noexec nowrite progbits