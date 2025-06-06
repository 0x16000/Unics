ENTRY(_start)

/* Define the end of the kernel */
PROVIDE(end = .);

PHDRS {
    headers PT_PHDR PHDRS;
    text PT_LOAD FILEHDR PHDRS FLAGS(5); /* Read + Execute */
    data PT_LOAD FLAGS(6); /* Read + Write */
}

SECTIONS {
    /* Kernel load address */
    . = 1M;

    /* Multiboot header must be first */
    .multiboot ALIGN(4) : {
        KEEP(*(.multiboot))
    } :text

    .text ALIGN(4K) : {
        *(.text .text.*)
    } :text

    .rodata ALIGN(4K) : {
        *(.rodata .rodata.*)
    } :text

    .data ALIGN(4K) : {
        *(.data .data.*)
    } :data

    .bss ALIGN(4K) : {
        *(COMMON)
        *(.bss .bss.*)
        __bitmap_start = .;
    } :data

    /* Add this to ensure proper alignment at the end */
    . = ALIGN(4K);
    _kernel_end = .;

    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
    }
}