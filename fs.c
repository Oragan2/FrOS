
#include "fs.h"
#include <stdint.h>
#include "disk.h"
#include "io.h"
#include "mem.h"

#define LBA_OFFSET 50

int initFRFS(struct FRFS *filesys) {
	uint8_t buffer[512];
	ata_read_sector(LBA_OFFSET,buffer);

	filesys = (struct FRFS*)buffer;
	if (filesys->magic[0] != 'F' || filesys->magic[1] != 'R' || filesys->magic[2] != 'F' || filesys->magic[3] != 'S') return 1;
	int rootIndex = 0;
	for (int i = 0; i < filesys->nbFiles; ++i) {
		struct FILE* tmpF = (struct FILE*)&buffer[8+i*sizeof(struct FILE)];
		filesys->root[rootIndex++] = *tmpF;
	}
	return 0;
}

char* readFile(struct FILE* file) {
	uint8_t buffer[512];
	ata_read_sector(LBA_OFFSET+file->startSec,buffer);
	return buffer;
}
