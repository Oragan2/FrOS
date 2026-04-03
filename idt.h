#ifndef IDT_H
#define IDT_H
#include <stdint.h>


struct IDTEntry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_high;
} __attribute__((packed));

struct IDTPtr {
    uint16_t limit;
    uint32_t base;
} __attribute((packed));

void idt_init();
void idt_set_gate(uint8_t num, uint32_t handler, uint16_t selector, uint8_t flags);

#endif
