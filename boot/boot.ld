/* Simple linker script for the bootloader.
   See the GNU ld 'info' manual ("info ld") to learn the syntax. */

/*
OUTPUT_FORMAT("elf64-x86-64", "elf64-x86-64", "elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
*/
OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)

ENTRY(start)

SECTIONS {
    . = 0x7c00;
    .stage_1 : {
        /* first stage */
        *(.boot-first-stage)
    }

    _rest_of_bootloader_start_addr = .;

    .rest_of_bootloader : {
        /* rest of bootloader */
        *(.text .text.*)
        *(.rodata .rodata.*)
        *(.data .data.*)
        *(.got)
    }

    . = ALIGN(0x1000);
    .page_table : {
        *(.entrypgdir)
    }

    . = ALIGN(512);
    _rest_of_bootloader_end_addr = .;
}
