#ifndef STRING_H
#define STRING_H

#include <stddef.h>

size_t strlen(const char *str);
char *strcpy(char *dest, const char *src);
char *strcat(char *dest, const char *src);
size_t strpos(const char *haystack, const char needle);
char *strcut(const char *src, size_t n, int part);
int strcmp(const char *str1, const char *str2);

#endif // STRING_H