#include "mem.h"
#include "fs.h"
#include <stdint.h>
#include "disk.h"
#include "io.h"

#define LBA_OFFSET 50

// Made a global variable since I don't want to make a malloc for now
struct FRFS filesys;

int initFRFS(struct FRFS *filesys) {
    uint8_t buffer[512];
    readSector(LBA_OFFSET, buffer);
    // check the magic bytes
    if (buffer[0] != 'F' || buffer[1] != 'R' || buffer[2] != 'F' || buffer[3] != 'S') {
        return 1; 
    }
    // initialise what's needed
    filesys->nbFiles = *(uint16_t*)&buffer[6];
    filesys->version = *(uint16_t*)&buffer[4];

    uint8_t* file_ptr = buffer + 8; 

    for (int i = 0; i < filesys->nbFiles; ++i) {
        // Automaticly make the file with the help of a cast
        struct FILE* f = (struct FILE*)file_ptr;
        // Copy the data
        filesys->root[i] = *f;
	print(filesys->root[i].name);
	print("\n");
	// Move the pointer
        file_ptr += sizeof(struct FILE);
    }

    return 0;
}

void readFile(struct FILE* file, char* buffer) {
	if (file->size == 0) return;
	uint8_t tmpbuffer[512];
	int byte_read = 0;
	int32_t ptn = file->startSec;
	
	for (uint32_t i = 0; i < file->size; ++i) {
		readSector(LBA_OFFSET+ptn+1, tmpbuffer);
		ptn = *(uint32_t*)&tmpbuffer[508];
		for (int j = 0; j < 508; ++j) {
			buffer[byte_read++] = (char)tmpbuffer[j];
		}
	}
	buffer[byte_read] = '\0';
}
