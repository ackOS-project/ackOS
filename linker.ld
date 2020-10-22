ENTRY(_start)

SECTIONS
{
	. = 0x8000;
 
	.boot :
	{
		*(.multiboot_header)
	}

	.text : ALIGN(0x1000)
	{
		*(.text)
	}

	/* Read-only data. */
	.rodata : ALIGN(0x1000)
	{
		*(.rodata)
	}
 
	/* Read-write data */
	.data : ALIGN(0x1000)
	{
		*(.data)
	}
 
	/* Read-write data and stack */
	.bss : ALIGN(0x1000)
	{
		*(COMMON)
		*(.bss)
	}
}