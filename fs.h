#ifndef FS_H
#define FS_H

#include <stdint.h>

struct FILE {
	char name[32];
	uint32_t startSec;
	uint32_t size;
	uint8_t type;
	uint64_t parent_id;
} __attribute__((packed)); 

struct FRFS {
	char magic[4];
	uint16_t version;
	uint16_t nbFiles;
	struct FILE root[64];
} __attribute__((packed));

extern struct FRFS filesys;

int initFRFS(struct FRFS* filesys);
void readFile(struct FILE* file, char* buffer);

#endif
