#include <stdint.h>

#ifndef UTIL_H
#define UTIL_H

typedef struct RGB {
    int red, green, blue;
} RGB;

uint8_t htoi(char c);
void freeStrArr(char **arr, int len);

#endif