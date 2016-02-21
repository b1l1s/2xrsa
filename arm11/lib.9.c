#include "lib.9.h"
#include <inttypes.h>

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


