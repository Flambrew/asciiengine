#include <stdint.h>

#ifndef UTIL_H
#define UTIL_H

typedef struct RGB {
    uint8_t red, green, blue;
} RGB;

typedef struct huffmanNode {
    char symbol;
    uint8_t length;
    uint32_t code;
} HNODE;

uint8_t htoi(char c);
void freeStrArr(char **arr, int len);
int pow2(int x, int n);
void huffmanTree(HNODE *tree, uint32_t len);

#endif