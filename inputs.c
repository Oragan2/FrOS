#include "inputs.h"
#include "screen.h"
#include <stdint.h>

static const unsigned char scancode_to_ascii[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','a','z','e','r','t','y','u','i','o','p','[',']','\n',
    0, 'q','s','d','f','g','h','j','k','l','m','\'','`',
    0,'>','w','x','c','v','b','n','.','>',':','/',0,'*',0,' '
    // ... fill remaining keys if needed
};

unsigned char inb(uint16_t port) {
    unsigned char ret;
    __asm__ __volatile__ ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void keyboard_handler(void) {
    unsigned char status = inb(0x64);
    if (status & 1) {
        unsigned char key = inb(0x60);

        if (key & 0x80) {
            return;
        } else {
            // Key press event
            char letter = scancode_to_ascii[key];
            if (letter) {
                keyboardPrint(letter);
            }
        }
    }
}
