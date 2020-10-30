ENTRY(_start)

SECTIONS
{
	. = 1M;
 
	.boot :
	{
		KEEP(*(.multiboot_header))
	}

	.text :
	{
		*(.text)
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
