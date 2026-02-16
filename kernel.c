// kernel.c
#include "string.h"
#include "cmd.h"
#include "io.h"
#include "disk.h"
#include "mem.h"
#include <stdint.h>

#define FS_START_LBA 0x1200
#define FRFS_MAGIC "FRFS"

void kernel_main(void) {
    uint8_t buffer[512];

    ata_read_sector(FS_START_LBA, buffer);

    uint32_t magic;
    memcpy(&magic, buffer, sizeof(uint32_t));

    clear_screen();
    if (!memcmp(&magic,FRFS_MAGIC,4))
    {
    	print("File system couldn't load!\n");
    }
    else {
    	print("File system loaded\n");
    }
    print("Hello, World from Kernel!\n");
    setPrefix("> ");
    print("Started listening for keyboard input...\n");
    print("");
    while (1) {
        keyboard_handler();
    }
}
