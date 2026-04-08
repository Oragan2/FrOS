// kernel.c
#include "io.h"
#include "fs.h"
#include <stdint.h>
#include "pci.h"
#include "idt.h"
#include "pic.h"
#include "pit.h"

#define FRFS_MAGIC "FRFS"

extern void irq0_stub();

void kernel_main(void) {
    *((char*)0xB8004) = '1'; // Breadcrumb 1
    idt_init();

    *((char*)0xB8004) = '2'; // Breadcrumb 1
    idt_set_gate(32, (uint32_t)irq0_stub, 0x08, 0x8E); // IRQ0 = interrupt 32
    
    *((char*)0xB8004) = '3'; // Breadcrumb 1
    pic_remap();
    *((char*)0xB8004) = '4'; // Breadcrumb 1
    __asm__ volatile("sti");
    *((char*)0xB8004) = '5'; // Breadcrumb 1
    pit_init();
    *((char*)0xB8004) = '6'; // Breadcrumb 1
    
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
    __asm__ volatile("cli"); // Disable interrupts
    pci_scan();
    __asm__ volatile("sti"); // Re-enable interrupts
    while (1) {__asm__ volatile("hlt");}
}
