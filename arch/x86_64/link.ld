ENTRY(_start)

SECTIONS
{
	. = 1M;

	.text :
	{
		KEEP(*(.multiboot))
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
