#include "mem.h"
#include <stdint.h>

#define KERNEL_VIRTUAL_SPACE 0xFFFFFFF800000000
#define TO_PHYS(addr) ((uint64_t)(addr) & 0xFFFFFFF)

#define HDDM_PML4_ADDR 256
#define MAX_GB 32

//bump allocator
static unsigned char heap[0x100000];
static unsigned int heap_ptr = 0;
unsigned char phys_bitmap[MAX_FRAMES/8];

//MMU paging
uint64_t hddm_pml4[512] __attribute__((aligned(4096)));
uint64_t hddm_pdpt[512] __attribute__((aligned(4096)));
uint64_t hddm_pds[MAX_GB][512] __attribute__((aligned(4096)));

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
	for (unsigned int i = 0; i < size; ++i) 
		if (((uint8_t*)address)[i] != 0) return 1;

	return 0;
}

void memset(void* dest, char val, unsigned int size) {
	uint64_t* p64 = dest;
	uint64_t patern = (uint8_t)val;
	patern |= patern << 8;
	patern |= patern << 16;
	patern |= patern << 32;

	while (size >= 8) {
		*p64++ = patern;
		size -= 8;
	}

	uint8_t* p8 = (uint8_t*)p64;

	while (size--)
		*p8++ = (uint8_t)val;
}

void pagingSetup() {
	memset(hddm_pdpt, 0, 4096);
	memset(hddm_pds, 0, 4096);

	hddm_pml4[0] = TO_PHYS(hddm_pdpt) | 0x3;
	
	unsigned int current_addr = 0;

	for (int gb = 0; gb < MAX_GB; ++gb) {
		hddm_pdpt[gb] = TO_PHYS(&hddm_pds[gb]) | 0x3; // writable and accesible

		for (int i = 0; i < 512; ++i) {
			hddm_pds[gb][i] = current_addr | 0x83;
			current_addr += 2*(1024*1024);
		}
	}

	// load register
	__asm__ volatile (".intel_syntax noprefix\n\t"
			"mov cr3, %0\n\t"
			".att_syntax prefix\n\t"
			: 
			: "r"(TO_PHYS(hddm_pml4))
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
