#ifndef _MEMORY
#define _MEMORY
#include <stdint.h>

#define NULL 0x0


void init_memory(uint32_t mem_ptr, uint32_t size);


void* malloc(uint32_t size);

void free(void* ptr);

void dump();

#endif
