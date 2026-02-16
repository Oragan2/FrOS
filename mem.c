#include "mem.h"
#include <stdint.h>

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
