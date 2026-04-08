#include "idt.h"
#include <stdint.h>

#define IDT_SIZE 256

static struct IDTEntry idt[IDT_SIZE];
static struct IDTPtr idt_ptr;

void idt_set_gate(uint8_t num, uint32_t handler, uint16_t selector, uint8_t flags) {
    idt[num].offset_low = handler & 0xFFFF;
    idt[num].offset_high = (handler >> 16) & 0xFFFF;
    idt[num].selector = selector;
    idt[num].zero = 0;
    idt[num].type_attr = flags;
}

void idt_init() {
    idt_ptr.limit = sizeof(idt) -1;
    idt_ptr.base = (uint32_t)&idt;

    for (int i = 0; i < IDT_SIZE; ++i) {
        idt_set_gate(i,0,0,0);
    }

    for (int i = 0; i < 256; i++) {
        extern void exception_handler(void);
        idt_set_gate(i, (uint32_t)exception_handler, 0x08, 0x8E);
    }

    extern void irq0_stub(void);
    idt_set_gate(32, (uint32_t)irq0_stub, 0x08, 0x8E);

    extern void idt_load(struct IDTPtr*);
    idt_load(&idt_ptr);
}
