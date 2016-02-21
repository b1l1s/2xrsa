#include <inttypes.h>
#include "constants.h"
#include "pointers.9.h"
#include "IFile.9.h"
#include "lib.9.h"

void core_main(void);

int __attribute__ ((section (".text.a9.entry"), naked)) core_start()
{
	// Taken from code.c@yifanlu/spider3dstools
	asm volatile  (".word 0xe1a00000");
	core_main();
	// should never be reached
	asm volatile ("bx lr");
}

void __attribute__ ((section (".text.a9"), naked)) svcSleepThread9(uint32_t nanoSecsLo, uint32_t nanoSecsHi)
{
	asm
	(
		"svc 0x0A\n\t"
		"bx lr"
	);
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
		
		"mrc p15, 0, r0, c5, c0, 3\n\t"
		"bic r0, r0, #0xF00000\n\t" // Region 5 protect
		"orr r0, r0, #0x300000\n\t" // Region 5 r/w access
		"mcr p15, 0, r0, c5, c0, 3\n\t"
		:::"r0"
	);
}

void __attribute__ ((section (".text.a9"))) flushCache()
{
	asm
	(
		"mov r0, #0\n\t"
		"mcr p15, 0, r0, c7, c5, 0\n\t"// Flush instruction cache
		"mov r1, #0\n"
	"flushloop0:\n\t"
		"mov r0, #0\n"
	"flushloop1:\n\t"
		"orr r2, r1, r0\n\t"
		"mcr p15, 0, r2, c7, c14, 2\n\t"// Clean and flush data cache entry (index and segment)
		"add r0, r0, #0x20\n\t"			// Increment line
		"cmp r0, #0x400\n\t"
		"bne flushloop1\n\t"
		"add r1, r1, #0x40000000\n\t"	// Increment segment counter
		"cmp r1, #0\n\t"
		"bne flushloop0\n\t"
		"bx lr"
		:::"r0","r1","r2", "memory"
	);
}

void __attribute__ ((section (".text.a9"))) nop_sub()
{}

void __attribute__ ((section (".text.a9"))) disableInterrupts()
{
	asm
	(
		"mrs r1, cpsr\n\t"
		"orr r1, r1, #0xC0\n\t" // Disable FIQ and IRQ interrupts
		"msr cpsr_c, r1\n\t"
		:::"r1"
	);
	
	register uint32_t nopsubptr asm ("r0") = 0;
	asm
	(
		"adr r0, nop_sub\n\t"
		:::
	);
	
	// nop the interrupt vector
	uint32_t* v0 = (uint32_t*)0x8000000;
	v0[0x01] = nopsubptr;
	v0[0x05] = nopsubptr; // svc
	v0[0x07] = nopsubptr;
	v0[0x09] = nopsubptr;
	v0[0x0B] = nopsubptr;
	
	flushCache();
}

void __attribute__ ((section (".text.a9"))) jump(void)
{
	disableInterrupts();
	((void (*)())0x23F00000)();
}

void __attribute__ ((section (".text.a9"))) core_main(void)
{
	asm volatile
	(
		"adr r0, flushCache\n\t"
		"bl svcBackdoor\n\t"
		:::"r0"
	);
	
	// Mutex, ensure only one ARM9 thread executes this function
	// just in case this is needed.
	uint32_t* single = (uint32_t*)0x23FFFE0C;
	if(*single == 0xAABBCCDD)
		while(1)
			svcSleepThread9(0x1DCD6500, 0);
	*single = 0xAABBCCDD;
	

	uint32_t *cur = (uint32_t *)0x20000000;
	
	asm volatile
	(
		"adr r0, mpuPerm\n\t"
		"bl svcBackdoor\n\t"
		:::"r0"
	);

	do {
		/* "FUCKFUCK" */
		if (*cur++ == MAGIC_WORD && *cur++ == MAGIC_WORD)
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
	
	/* Size is completely arbitrary */
	// Find and assign the fb addresses
	*((unsigned int*)0x23FFFE00) = cur[0];
	*((unsigned int*)0x23FFFE04) = cur[1];
	*((unsigned int*)0x23FFFE08) = cur[2];
	/*
	*((unsigned int*)0x080FFFC0) = cur[0];
	*((unsigned int*)0x080FFFD4) = cur[2];
	*((unsigned int*)0x080FFFD8) = 0;*/

	asm volatile
	(
		"adr r0, jump\n\t"
		"bl svcBackdoor\n\t"
		:::"r0"
	);
}
