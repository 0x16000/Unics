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
extern main
global multiboot_info_ptr

_start:
    ; Set up stack
    mov esp, stack_top
    
    ; Save multiboot info pointer
    mov [multiboot_info_ptr], ebx
    
    ; Clear direction flag
    cld
    
    ; Call kernel main
    call main
    
    ; Halt if main returns
    cli
.hang:
    hlt
    jmp .hang

section .bss
align 16
stack_bottom:
    resb 16384                 ; 16KB stack
stack_top:

multiboot_info_ptr: 
    resd 1                     ; Reserve 4 bytes

section .note.GNU-stack noalloc noexec nowrite progbits