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

void __attribute__ ((section (".text.a9"))) core_main(void)
{
	uint32_t *cur = (uint32_t *)0x20000000;
	
	asm volatile
	(
		"adr r0, mpuPerm\n\t"
		"bl svcBackdoor\n\t"
		:::"r0"
	);

	for (int i = 0; i < 0x00038400; ++i) {
			((uint8_t*)0x1848F000)[i] = 0x01;
			((uint8_t*)0x184C7800)[i] = 0x01;
	}

	do {
		/* "FUCK" */
		if (*cur++ == 0x4b435546)
			break;
	} while ((uintptr_t)cur < (uintptr_t)0x28000000);

	if ((uintptr_t)cur == (uintptr_t)0x28000000) {
		/* Not found, let's just hang ourselves. */
		while(1);
	}

	/* Due to the post-increment in the loop above, "cur" already points
	 * at the right position.
	 *
	 * Size is completely arbitrary,
	 */
	xmemcpy((void*)0x23F00000, cur, 0x10000);

	((void (*)())0x23F00000)();
}