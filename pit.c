#include "output.h"
#include "pit.h"
#include <stdint.h>
#include "pic.h"

#define PIT_CMD 0x43
#define PIT_CH0 0x40
#define PIT_HZ 100 // 100Hz = 10ms tick

static uint64_t tick = 0;

void pit_handler() {
    ++tick;
    if (tick % 100 == 0) {
        print(".");
    }
    pic_send_eoi(0);
}

void pit_init() {
    uint16_t divisor = 1193182 / PIT_HZ; //PIT base clock is 1.193182

    outb(PIT_CMD, 0x36);
    inb(0x80);
    outb(PIT_CH0, divisor & 0xFF);
    outb(PIT_CH0, (divisor >> 8) & 0xFF);
}

uint32_t pit_get_tick() {return tick;}
