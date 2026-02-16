#include <stdint.h>
#define ATA_DATA       0x1F0
#define ATA_SECTOR_CNT 0x1F2
#define ATA_LBA_LOW    0x1F3
#define ATA_LBA_MID    0x1F4
#define ATA_LBA_HIGH   0x1F5
#define ATA_DRIVE      0x1F6
#define ATA_COMMAND    0x1F7
#define ATA_STATUS     0x1F7

#define ATA_CMD_READ   0x20

#include "inputs.h"
#include "output.h"

static void ata_wait()
{
    while (inb(ATA_STATUS) & 0x80); // wait while busy
}

void ata_read_sector(uint32_t lba, uint8_t* buffer)
{
    ata_wait();

    outb(ATA_SECTOR_CNT, 1);
    outb(ATA_LBA_LOW,  lba);
    outb(ATA_LBA_MID,  lba >> 8);
    outb(ATA_LBA_HIGH, lba >> 16);
    outb(ATA_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));

    outb(ATA_COMMAND, ATA_CMD_READ);
    ata_wait();

    for (int i = 0; i < 256; i++)
        ((uint16_t*)buffer)[i] = inw(ATA_DATA);
}
