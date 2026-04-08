ENTRY(_start)
OUTPUT_FORMAT("binary")

SECTIONS
{
    . = 0x7E00;

    /* Combine CODE and STRINGS into one block */
    .all_code_and_data : {
        *(.entry)
        *(.text .text.*)
        *(.rodata .rodata.*)
        *(.data .data.*)
    }

    /* Keep BSS at the end (it's not in the file) */
    .bss : ALIGN(4) {
        __bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        __bss_end = .;
    }
}
