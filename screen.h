#ifndef SCREEN_H
#define SCREEN_H

void print_char(unsigned char c, unsigned char color, int pos);
void clear_screen(void);
void scroll_screen(void);
void print(const char *s);
void showScreen(void);
void newLine(void);

#endif // SCREEN_H