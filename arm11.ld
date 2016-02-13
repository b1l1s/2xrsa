OUTPUT_ARCH(arm)
ENTRY(_main)
SECTIONS
{
	. = 0x00100000;
	.text : {
		*(.text)
		arm9_stage1 = .;
		*(.text.a9.entry)
		*(.text.a9)
		arm9_stage1_end = .;
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