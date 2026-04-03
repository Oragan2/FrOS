// kernel.c
#include "io.h"
#include "fs.h"
#include <stdint.h>
#include "pci.h"
#include "idt.h"
#include "pic.h"
#include "pit.h"

#define FS_START_LBA 9 
#define FRFS_MAGIC "FRFS"

extern void irq0_stub();

void kernel_main(void) {
    pic_remap();
    idt_init();

    idt_set_gate(32, (uint32_t)irq0_stub, 0x08, 0x8E); // IRQ0 = interrupt 32
    
    pit_init();

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
    pci_scan();
    while (1) {
        keyboard_handler();
    }
}
