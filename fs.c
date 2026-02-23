#include "mem.h"
#include "fs.h"
#include <stdint.h>
#include "disk.h"
#include "string.h"
#include "io.h"

#define LBA_OFFSET 50

// Made a global variable since I don't want to make a malloc for now
struct FRFS filesys;
uint8_t sector_map[512];

void set_sector_used(uint32_t lba) {
    sector_map[lba / 8] |= (1 << (lba % 8));
}

int is_sector_free(uint32_t lba) {
    return !(sector_map[lba / 8] & (1 << (lba % 8)));
}

uint32_t find_first_free() {
    for (uint32_t i = 0; i < 4096; i++) {
        if (is_sector_free(i)) return i;
    }
    return 0; // Disk full!
}

int initFRFS(struct FRFS *filesys) {
    uint8_t buffer[512];
    uint8_t obuffer[512];
    readSector(LBA_OFFSET, buffer);
    // check the magic bytes
    if (buffer[0] != 'F' || buffer[1] != 'R' || buffer[2] != 'F' || buffer[3] != 'S') {
        return 1; 
    }
    // initialise what's needed
    strcpy(filesys->magic, "FRFS");
    filesys->nbFiles = *(uint16_t*)&buffer[6];
    filesys->version = *(uint16_t*)&buffer[4];

    uint8_t* file_ptr = buffer + 8; 

    for (int i = 0; i < filesys->nbFiles; ++i) {
        // Automaticly make the file with the help of a cast
        struct FILE* f = (struct FILE*)file_ptr;
        // Copy the data
        filesys->root[i] = *f;
	// Move the pointer
	set_sector_used(filesys->root[i].startSec);
	if (f->size != 1) {
		uint32_t ptn = f->startSec;
		for (uint32_t i = 0; i < f->size; ++i) {
			readSector(LBA_OFFSET+ptn+1, obuffer);
			ptn = *(uint32_t*)&obuffer[508];
			if (ptn != 0) 
				set_sector_used(ptn);
		}
	}
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

void saveFile(const char* name) {
	uint8_t buffer[512];
	strcpy(filesys.root[filesys.nbFiles++].name, name);
	filesys.root[filesys.nbFiles].size = 0;
	filesys.root[filesys.nbFiles].startSec = find_first_free();
	filesys.root[filesys.nbFiles].type = 0;
	writeSector(LBA_OFFSET, (uint8_t*)&filesys);
}

void writeFile(struct FILE* file, const char* data) {
	uint8_t buffer[508];
	uint32_t ptn = file->startSec;
	for (int bytes_count = 0; data[bytes_count];) {
		if (is_sector_free(ptn)) {
			set_sector_used(ptn);
		}
		for (int i = 0; data[bytes_count]; ++i) {
			buffer[i] = (uint8_t)data[bytes_count++];
		}
		for (int i = bytes_count; i < 508; ++i)
			buffer[i] = '\0';
		if (data[bytes_count]) {
			*(uint32_t*)buffer[508] = ptn;	
		}
		writeSector(ptn+LBA_OFFSET+1, buffer);
		ptn = find_first_free();
	} 
}

struct FILE* findFile(const char * filename) {
	for (int i = 0; i < filesys.nbFiles; ++i) {
		if (!strcmp(filesys.root[i].name,filename)) {print(filesys.root[i].name);return &(filesys.root[i]);}
	}
	return NULL;
}
