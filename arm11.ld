OUTPUT_ARCH(arm)
ENTRY(_main)
SECTIONS
{
	. = 0x00100000;
	.text : {
		*(.text)
	}
	.data : {
		*(.data)
	}
	.bss : {
		*(.bss COMMON)
	}
	.rodata : {
		*(.rodata)
	}
	. = ALIGN(4);
}