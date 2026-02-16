#ifndef INPUTS_H
#define INPUTS_H

#include "stdint.h"

unsigned char inb(unsigned short port);
static inline uint16_t inw(uint16_t port)
{
    uint16_t ret;
    __asm__ volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
void keyboard_handler(void);

#endif // INPUTS_H
