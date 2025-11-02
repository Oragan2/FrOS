#ifndef SCREEN_H
#define SCREEN_H

void print_char(unsigned char c, int pos);
void clear_screen(void);
void scroll_screen(void);
void setPrefix(const char *s);
void setColor(unsigned short c);
void print(const char *s);
void newLine(void);
void keyboardPrint(const char c);

#endif // SCREEN_H