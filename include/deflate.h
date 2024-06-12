#include <stdint.h>
#include <stdlib.h>

#ifndef DEFLATE_H
#define DEFLATE_H

typedef struct huffmanNode {
    char symbol;
    uint8_t length;
    uint32_t code;
} HNODE;

void huffmanTree(HNODE *tree, uint32_t len);

#endif