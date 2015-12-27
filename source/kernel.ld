ENTRY(_start)

SECTIONS
{
    /* Starts at LOADER_ADDR. */
    . = 0x8000;
    __start = .;
    __text_start = .;
    .text :
    {
        KEEP(*(.text.boot))
        *(.text)
    }
    __text_end = .;
    . = ALIGN(4096); /* align to page size */

    __init_array_start = .;
    .init_array :
    {
        KEEP(*(.init_array*))
    }
    __init_array_end = .;
    . = ALIGN(4096); /* align to page size */

    __rodata_start = .;
    .rodata :
    {
        *(.rodata)
    }
    __rodata_end = .;
    . = ALIGN(4096); /* align to page size */

    __data_start = .;
    .data :
    {
        *(.data)
    }
    __data_end = .;
    . = ALIGN(4096); /* align to page size */

    __bss_start = .;
    .bss :
    {
        bss = .;
        *(.bss)
    }
    __bss_end = .;
    __end = .;
}
