#include "screen.h"
#include "string.h"
#include "cmd.h"

static char screen[25*80] = { 0 };
static char buffer[80] = { 0 };
static int bufferIndex = 0;
static char prefix[5] = { 0 };
unsigned short color = 0x0F;
static int cursorX = 0;
static int cursorY = 0;

void print_char(unsigned char c, int pos) {
    volatile unsigned short *vga = (volatile unsigned short *)0xB8000;
    vga[pos] = (unsigned short)((((unsigned short)color) << 8) | (unsigned short)c);
}

void clear_screen(void) {
    for (int i = 0; i < 80 * 25; ++i)
        print_char(' ', i);
    cursorX = 0;
    cursorY = 0;
}

void scroll_screen(void) {
    for (int r = 0; r < 24 * 80; ++r)
        screen[r] = screen[r + 80];
    for (int r = 24 * 80; r < 25 * 80; ++r)
        screen[r] = ' ';
}

void setPrefix(const char *s) {
    if (strlen(s) >= 5) return;
    strcpy(prefix, s);
    prefix[strlen(s)] = '\0';
    print(prefix);
}

void setColor(unsigned short c) {
    color = c;
}

void newLine(void) {
    screen[cursorY*80+cursorX] = '\0';
    buffer[bufferIndex] = '\0';
    cursorX = 0;
    bufferIndex = 0;
    ++cursorY;
    if (cursorY >= 25) {
        scroll_screen();
        cursorY = 24;
    }
}

unsigned short swapBytes(unsigned short a) {
    return (a >> 4) | (a << 4);
}

void print(const char *s) {
    while (*s) {
        if (*s == '\n' || cursorX >= 79) {
            newLine();
        }
        else if (*s == '\b' && cursorX >= 1) {
            --cursorX;
            screen[cursorY*80+cursorX] = ' ';
            print_char(' ', cursorY*80+cursorX);
        }
        else if (*s != '\b') {
            screen[cursorY*80+cursorX] = *s;
            if (cursorX < 79) ++cursorX;
            print_char(*s, cursorY*80+cursorX-1);
        }
        ++s;
    }
}

void keyboardPrint(char c) {
    if (c == '\n' || cursorX >= 79) {
        newLine();
        cmdCheck(buffer);
        print(prefix);
    }
    else if (c == '\b' && cursorX >= strlen(prefix)+1) {
        --cursorX;
        --bufferIndex;
        screen[cursorY*80+cursorX] = ' ';
        buffer[bufferIndex] = ' ';
        print_char(' ', cursorY*80+cursorX);
    }
    else if (c != '\b') {
        screen[cursorY*80+cursorX] = c;
        buffer[bufferIndex] = c;
        if (cursorX < 79) {++cursorX; ++bufferIndex;}
        print_char(c, cursorY*80+cursorX-1);
    }
}
