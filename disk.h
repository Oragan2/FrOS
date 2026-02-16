#ifndef DISK_H
#define DISK_H
#include "stdint.h"

static void ata_wait();
void ata_read_sector(uint32_t lba, uint8_t* buffer);

#endif
