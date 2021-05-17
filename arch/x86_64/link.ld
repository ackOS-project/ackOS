ENTRY(_start)

SECTIONS
{
	. = 1M;
	_kernel_start = .;

	.multiboot2 ALIGN(4K) :
	{
		KEEP(*(.multiboot2))
	}

	.stivale2hdr ALIGN(4K) :
	{
		KEEP(*(.stivale2hdr))
	}

	.text ALIGN(4K) :
	{
		KEEP(*(.text*))
	}

	.rodata ALIGN(4K) :
	{
		KEEP(*(.rodata*))
	}

	.data ALIGN(4K) :
	{
		KEEP(*(.data*))
	}

	.bss ALIGN(4K) :
    {
		KEEP(*(COMMON))
        KEEP(*(.bss*))
    }

	_kernel_end = .;
}
