#include "mem.h"
#include <stdint.h>

static unsigned char heap[0x100000];
static unsigned int heap_ptr = 0;
unsigned char phys_bitmap[MAX_FRAMES/8];

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

void* kmalloc(unsigned int size) {
    size = (size + 3) & ~3;
    if (heap_ptr + size > sizeof(heap)) return 0;
    void * ptr = &heap[heap_ptr];
    heap_ptr += size;
    return ptr;
}
