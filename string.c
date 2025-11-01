#include "string.h"

int strcmp(const char *str1, const char *str2) {
    while (*str1 && *str2) {
        if (*str1 != *str2)
            return 0;
        ++str1;
        ++str2;
    }
    return (*str1 == *str2) ? 1 : 0;
}

size_t strlength(const char *str) {
    size_t len = 0;
    while (*str++) {
        ++len;
    }
    return len;
}

char *strcopy(char *dest, const char *src) {
    char *original_dest = dest;
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
    return original_dest;
}

char *strconcat(char *dest, const char *src) {
    char *original_dest = dest;
    while (*dest) {
        ++dest;
    }
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
    return original_dest;
}

void *strcut(const char *src, size_t n, int part, char *result) {
    size_t rlen = strlength(src);
    if (part != 1 && part != 2) return NULL;
    if (part == 1) {
        for (size_t i = 0; i < n && src[i]; ++i) {
            result[i] = src[i];
        }
        result[n] = '\0';
    }
    else {
        for (size_t i = n; i < rlen && src[i]; ++i) {
            result[i - n] = src[i];
        }
        result[rlen - n] = '\0';
    }
}

size_t strpos(const char *haystack, const char needle) {
    size_t hlen = strlength(haystack);
    if (needle == '\0') return 0;
    for (size_t i = 0; i < hlen; ++i) {
        if (haystack[i] == needle)
            return i+1;
    }
    return -1; // Not found
}