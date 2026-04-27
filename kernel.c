// kernel.c
#include "io.h"
#include "fs.h"
#include <stdint.h>
#include "pci.h"
#include "idt.h"
#include "pic.h"
#include "pit.h"
#include "debug.h"

extern void irq0_stub();

void kernel_main(void) {
    debug_println("[BOOT] IDT init");
    idt_init();
    idt_set_gate(32, (uint32_t)irq0_stub, 0x08, 0x8E); // IRQ0 = interrupt 32
    
    debug_println("[BOOT] PIC remap"); 
    pic_remap();
    pit_init();
    
    clear_screen();
    if (initFRFS(&filesys)) {
    	debug_println("[FAIL] FRFS load failed");   
    }
    else debug_println("[OK] FRFS loaded");
    debug_println("[OK] BOOT COMPLETE");
    pci_scan();
    __asm__ volatile("sti"); // enable interrupts

    #ifdef TESTING
    	debug_println("[BOOT] BOOT ENDED EXITING");
	outl(0xF4,0);
    #endif

    while (1) {__asm__ volatile("hlt");}
}
