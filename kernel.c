// kernel.c
#include "io.h"
#include "fs.h"
#include <stdint.h>

#define FS_START_LBA 9 
#define FRFS_MAGIC "FRFS"

void kernel_main(void) {
    clear_screen();
    if (initFRFS(&filesys)) {
	setColor(0x04);
	print("FRFS has failed to properly load\n");
	setColor(0x0F);
    }
    else print ("FRFS has been properly loaded\n");
    print("Hello, World from Kernel!\n");
    print("Started listening for keyboard input...\n");
    print("");
    while (1) {
        keyboard_handler();
    }
}
