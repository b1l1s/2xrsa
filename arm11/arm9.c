#include <inttypes.h>

void core_main(void);

int __attribute__ ((section (".text.a9.entry"), naked)) core_start()
{
	// Taken from code.c@yifanlu/spider3dstools
	asm volatile  (".word 0xe1a00000");
	core_main();
	// should never be reached
	asm volatile ("bx lr");
}

/* Slow but safe, from CakesROPSpider */
void __attribute__ ((section (".text.a9"))) xmemcpy(void* dst, const void* src, uint32_t size)
{
	char *destc = (char *) dst;
	const char *srcc = (const char *) src;
	for(uint32_t i = 0; i < size; i++)
	{
		destc[i] = srcc[i];
	}
}

void __attribute__ ((section (".text.a9"))) xmemset(void* addr, int val, unsigned int size)
{
	char* caddr = (char*) addr;
	while(size--)
		*(caddr++) = val;
}

void __attribute__ ((section (".text.a9"), naked)) svcBackdoor(void* funcptr)
{
	asm volatile
	(
		"svc 0x7B\n\t"
		"bx lr"
	);
}

void __attribute__ ((section (".text.a9"))) mpuPerm()
{
	asm volatile
	(
		"ldr r0, =0x10000037\n\t"
		"mcr p15, 0, r0, c6, c3, 0\n\t"	// Region 3, 0x10000000-0x20000000
		
		"mov r0, #0\n\t"
		"mcr p15, 0, r0, c6, c4, 0\n\t"	// Region 4, disable
		
		"mcr p15, 0, r0, c6, c6, 0\n\t"	// Region 6, disable
		"mcr p15, 0, r0, c6, c7, 0\n\t"	// Region 7, disable
		:::"r0"
	);
}

typedef struct IFILE
{
	uint32_t unk[4];
	uint64_t fptr;
	uint64_t size;
} IFILE;

void __attribute__ ((section (".text.a9"))) data_dump()
{
	int(*IFile_Open)(IFILE* file, const wchar_t* path, uint32_t flags) = (void*)0x08065AA5;
	void(*IFile_Write)(IFILE* file, uint32_t* written, const void* buffer, uint32_t size, uint32_t blockSize) = (void*)0x08068719;
	
	uint32_t tname[7];
	tname[0] = 0x00640073;
	tname[1] = 0x0063006D;
	tname[2] = 0x002F003A;
	tname[3] = 0x00390061;
	tname[4] = 0x002E0066;
	tname[5] = 0x00690062;
	tname[6] = 0x0000006E;
	
	IFILE file;
	uint32_t written;
	
	xmemset(&file, 0, sizeof(file));
	IFile_Open(&file, (const wchar_t*)tname, 6);
	IFile_Write(&file, &written, (const uint32_t*)0x10012000, 0x00000100, 1);
}

void __attribute__ ((section (".text.a9"))) core_main(void)
{
	uint32_t *cur = (uint32_t *)0x20000000;
	
	asm volatile
	(
		"adr r0, mpuPerm\n\t"
		"bl svcBackdoor\n\t"
		:::"r0"
	);
	
	data_dump();

	do {
		/* "FUCK" */
		if (*cur++ == 0x4b435546)
			break;
	} while ((uintptr_t)cur < (uintptr_t)0x28000000);

	if ((uintptr_t)cur == (uintptr_t)0x28000000) {
		/* Not found, let's just hang ourselves. */
		while(1)
		for (int i = 0; i < 0x00038400; ++i) {
			((uint8_t*)0x1848F000)[i] = 0x00;
			((uint8_t*)0x184C7800)[i] = 0x00;
		}
	}
	
	while(1)
	for (int i = 0; i < 0x00038400; ++i) {
		((uint8_t*)0x1848F000)[i] = 0xFF;
		((uint8_t*)0x184C7800)[i] = 0xFF;
	}

	/* Due to the post-increment in the loop above, "cur" already points
	 * at the right position.
	 *
	 * Size is completely arbitrary,
	 */
	xmemcpy((void*)0x23F00000, cur, 0x10000);

	((void (*)())0x23F00000)();
}