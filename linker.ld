ENTRY(_start)

SECTIONS
{
	. = 1M;

	.text :
	{
		KEEP(*(.multiboot_header))
		*(.text*)
		*(.rodata*)
	}

	.data :
	{
		*(.data)
	}

	.rodata :
	{
		*(.rodata)
	}

	.bss :
    {
		*(COMMON)
        *(.bss)
    }
}
