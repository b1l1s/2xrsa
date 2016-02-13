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


void __attribute__ ((section (".text.a9"))) core_main(void)
{
	uint32_t *cur = (uint32_t *)0x20000000;

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

