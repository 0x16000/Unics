ENTRY(_start)

SECTIONS {
    /* The kernel will be loaded at 1MB */
    . = 1M;

    .multiboot ALIGN(4K) : {
        /* Ensure multiboot header is at the beginning */
        *(.multiboot)
    }

    .text ALIGN(4K) : {
        *(.text)
    }

    .rodata ALIGN(4K) : {
        *(.rodata*)
    }

    .data ALIGN(4K) : {
        *(.data)
    }

    .bss ALIGN(4K) : {
        *(COMMON)
        *(.bss)
        
        /* Symbol for the end of kernel memory */
        __bitmap_start = .;
    }

    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
    }
}
