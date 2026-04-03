#include "screen.h"
#include "string.h"
#include "cmd.h"

static char screen[25*80] = { 0 };
static char buffer[80] = { 0 };
static int bufferIndex = 0;
static char prefix[5] = "> \0";
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

void reload_screen(void) {
	for (int i = 0; i < 25*80; ++i) {
		print_char(screen[i], i);
	}
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
	reload_screen();
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
	buffer[cursorX] = '\0';
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

void printHex(unsigned int n) {
    char hex[] = "0123456789ABCDEF";
    char buffer[9];
    buffer[8] = '\0';
    for (int i = 7; i >= 0; --i) {
        buffer[i] = hex[n & 0xF];
        n >>= 4;
    }
    print("0x");
    print(buffer);
} //prints a 32-bit number in hexadecimal format (8 digits). Used for debugging pci.

void printHex16(unsigned short n) {
    char hex[] = "0123456789ABCDEF";
    char buffer[5];
    buffer[4] = '\0';
    for (int i = 3; i >= 0; --i) {
        buffer[i] = hex[n & 0xF];
        n >>= 4;
    }
    print("0x");
    print(buffer);
} //prints a 16-bit number in hexadecimal format (4 digits). Used for debugging pci.


void printHex8(unsigned char n) {
    char hex[] = "0123456789ABCDEF";
    char buffer[3];
    buffer[2] = '\0';
    for (int i = 1; i >= 0; --i) {
        buffer[i] = hex[n & 0xF];
        n >>= 4;
    }
    print("0x");
    print(buffer);
} //prints an 8-bit number in hexadecimal format (2 digits). Used for debbugging pci.

void printNumber(unsigned int n) {
    char buffer[11];
    buffer[10] = '\0';
    int i = 9;
    if (n == 0) {
        print("0");
        return;
    }
    while (n > 0 && i >= 0) {
        buffer[i] = '0' + (n % 10);
        n /= 10;
        --i;
    }
    print(&buffer[i + 1]);
} //prints a number in decimal format. 