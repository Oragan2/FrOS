#ifndef MEM_H
#define MEM_H

#define MAX_FRAMES 800


void memcpy(void * dest, void * from, unsigned int size);
int memcmp(void * c1, void * c2, unsigned int size);
int memcheck(void * address, unsigned int size);
void memmanager_init();
void* kmalloc(unsigned int nbBytes);

#endif
