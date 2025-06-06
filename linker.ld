ENTRY(_start)

SECTIONS {
    /* Kernel load address */
    . = 1M;

    /* Multiboot header must be first and kept */
    .multiboot ALIGN(4) : {
        KEEP(*(.multiboot))
    }

    .text ALIGN(4) : {
        *(.text)
    }

    .rodata ALIGN(4) : {
        *(.rodata*)
    }

    .data ALIGN(4) : {
        *(.data)
    }

    .bss ALIGN(4) : {
        *(COMMON)
        *(.bss)
        __bitmap_start = .;
    }

    end = .;

    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
    }
}
