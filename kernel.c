// kernel.c
#include "string.h"
#include "cmd.h"
#include "io.h"
#include "disk.h"
#include "mem.h"
#include "fs.h"
#include <stdint.h>

#define FS_START_LBA 9 
#define FRFS_MAGIC "FRFS"

void kernel_main(void) {
    struct FRFS filesys;
    clear_screen();
    if (initFRFS(&filesys)) {
	setColor(0x04);
	print("FRFS has failed to properly load\n");
	setColor(0x0F);
    }
    else print ("FRFS has been properly loaded\n");
	
    print(filesys.root[1].name);
    print("\n");
    print("Hello, World from Kernel!\n");
    setPrefix("> ");
    print("Started listening for keyboard input...\n");
    print("");
    while (1) {
        keyboard_handler();
    }
}
