#include <stdint.h>

#ifndef UTIL_H
#define UTIL_H

typedef struct RGB {
    uint8_t red, green, blue;
} RGB;

uint8_t htoi(char c);
int pow2(int x, int n);

#endif