// debug.c
#include "output.h" // for outb

void debug_print(const char *s) {
    while (*s) {
        outb(0xE9, *s++);
    }
}

void debug_println(const char *s) {
    debug_print(s);
    outb(0xE9, '\n');
}
