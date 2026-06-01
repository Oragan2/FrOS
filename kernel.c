// kernel.c
#include "io.h"
#include "fs.h"
#include <stdint.h>
#include "pci.h"
#include "idt.h"
#include "pic.h"
#include "pit.h"

extern void irq0_stub();

void kernel_main(void) {
    idt_init();
    idt_set_gate(32, (uint32_t)irq0_stub, 0x08, 0x8E); // IRQ0 = interrupt 32
    
    pic_remap();
    pit_init();
    
    if (initFRFS(&filesys)) {
        print("FRFS found and loaded\n");
    }
    else print("FRFS not found\n");
    pci_scan();
    __asm__ volatile("sti"); // enable interrupts

	outl(0xF4,0);
	clear_screen();
    while (1) {__asm__ volatile("hlt");}
}
