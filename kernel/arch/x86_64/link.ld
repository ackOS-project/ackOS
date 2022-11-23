OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)

ENTRY(x86_begin)

PHDRS
{
    text PT_LOAD FLAGS((1 << 2) | (1 << 0)); /* Read + Execute */
    rodata PT_LOAD FLAGS((1 << 2)); /* Read only */
    data PT_LOAD FLAGS((1 << 2) | (1 << 1)); /* Read + Write */
}

SECTIONS
{
    /* We want to be placed at 2GiB (higher half) because that is what the limine protocol mandates */
    . = 0xffffffff80000000;

    .text : {
        *(.text .text.*)
    } :text

    . += CONSTANT(MAXPAGESIZE);

    .rodata : {
        *(.rodata .rodata.*)
    } :rodata

    . += CONSTANT(MAXPAGESIZE);

    .data : {
        *(.data .data.*)
    } :data

    .bss : {
        *(COMMON)
        *(.bss .bss.*)
    } :data

    /DISCARD/ : {
        *(.eh_frame)
        *(.note .note.*)
    }
}
