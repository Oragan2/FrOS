#ifndef DISK_H
#define DISK_H
#include "stdint.h"

static void ata_wait();
void readSector(uint32_t lba, uint8_t* buffer);

#endif
