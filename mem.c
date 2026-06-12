#include "mem.h"
#include <stdint.h>

#define KERNEL_VIRTUAL_SPACE 0xFFFFFFF
#define VIRT_TO_PHYS(addr) ((uint64_t)(addr) & KERNEL_VIRTUAL_SPACE)

//bump allocator
static unsigned char heap[0x100000];
static unsigned int heap_ptr = 0;
unsigned char phys_bitmap[MAX_FRAMES/8];

//MMU paging
uint64_t pml4[512] __attribute__((aligned(4096)));
uint64_t pdpt[512] __attribute__((aligned(4096)));
uint64_t pd[512] __attribute__((aligned(4096)));

void memcpy(void * dest, void * from, unsigned int size) {
	for (unsigned int i = 0; i < size; ++i) {
		((uint8_t*)dest)[i] = ((uint8_t*)from)[i];
	}
}

int memcmp(void * c1, void * c2, unsigned int size) {
	int retVal = 0;
	for (unsigned int i = 0; i < size; ++i) {
		if (((uint8_t*)c1)[i] < ((uint8_t*)c2)[i]) retVal = -1;
		else if (((uint8_t*)c1)[i] > ((uint8_t*)c2)[i]) retVal = 1;
	}
	return retVal;
}

int memcheck(void * address, unsigned int size) {
	for (unsigned int i = 0; i < size; ++i) {
		if (((uint8_t*)address)[i] != 0) return 1;
	}
	return 0;
}

void memset(void* dest, char val, unsigned int size) {
	for (int i = 0; i < size/8; i += 8)
		*(char*)(dest+i) = (char)val;
}

void pagingSetup() {
	memset(pml4, 0, 4096);
	memset(pdpt, 0, 4096);
	memset(pd, 0, 4096);

	pml4[0] = VIRT_TO_PHYS(pdpt) | 0x3;

	pdpt[0] = VIRT_TO_PHYS(pd) | 0x3;

	pd[0] = 0x000000 | 0x83;

	// load register
	__asm__ volatile (".intel_syntax noprefix\n\t"
			"mov cr3, %0\n\t"
			".att_syntax prefix\n\t"
			: 
			: "r"(VIRT_TO_PHYS(pml4))
			: "memory"
	);
}


// bump allocator
void* kmalloc(unsigned int size) {
    size = (size + 3) & ~3;
    if (heap_ptr + size > sizeof(heap)) return 0;
    void * ptr = &heap[heap_ptr];
    heap_ptr += size;
    return ptr;
}
