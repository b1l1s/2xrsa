#include "lib.11.h"

int _strlen(char* str)
{
	int l = 0;
	while(*(str++))l++;
	return l;
}

void _strcpy(char* dst, char* src)
{
	while(*src)*(dst++)=*(src++);
	*dst=0x00;
}

void _memset(void* addr, char val, unsigned int size)
{
	char* caddr = (char*) addr;
	while(size--)
		*(caddr++) = val;
}

