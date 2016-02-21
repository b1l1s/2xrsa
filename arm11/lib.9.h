#ifndef LIB_9_H_
#define LIB_9_H_

#include <inttypes.h>

void __attribute__ ((section (".text.a9"))) xmemcpy(void* dst, const void* src, uint32_t size);
void __attribute__ ((section (".text.a9"))) xmemset(void* addr, int val, unsigned int size);

#endif//LIB_9_H_

