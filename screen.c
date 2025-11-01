#include "screen.h"
#include "cmd.h"

static char screen[25*80] = { 0 };
static char buffer[80] = { 0 };
static int cursorX = 0;
static int cursorY = 0;
static int cmdIndex = 0;

void print_char(unsigned char c, unsigned char color, int pos) {
    volatile unsigned short *vga = (volatile unsigned short *)0xB8000;
    vga[pos] = (unsigned short)((((unsigned short)color) << 8) | (unsigned short)c);
}

void clear_screen(void) {
    for (int i = 0; i < 80 * 25; ++i)
        screen[i] = ' ';
    cursorX = 0;
    cursorY = 0;
}

void scroll_screen(void) {
    for (int r = 0; r < 24 * 80; ++r)
        screen[r] = screen[r + 80];
    for (int r = 24 * 80; r < 25 * 80; ++r)
        screen[r] = ' ';
}

void newLine(void) {
    screen[cursorY*80+cursorX] = '\0';
    buffer[cursorX] = '\0';
    cursorX = 0;
    ++cursorY;
    if (cursorY >= 25) {
        scroll_screen();
        cursorY = 24;
    }
}

void print(const char *s) {
    while (*s) {
        if (*s == '\n' || cursorX >= 79) {
            newLine();
            cmdCheck(buffer);
        }
        else if (*s == '\b' && cursorX >= 1) {
            --cursorX;
            screen[cursorY*80+cursorX] = ' ';
            buffer[cursorX] = ' ';
        }
        else if (*s != '\b') {
            screen[cursorY*80+cursorX] = *s;
            buffer[cursorX] = *s;
            if (cursorX < 79) ++cursorX;
        }
        ++s;
    }
    showScreen();
}

void showScreen(void) {
    int i, j;
    for (i=0; i<25; ++i) {
        for (j=0; j < 80 ; ++j) {
            if (cursorX == j && cursorY == i)
                print_char(screen[i*80+j], 0xF0, i*80+j);
            else
                print_char(screen[i*80+j], 0x0F, i*80+j);
        }
    }
}
