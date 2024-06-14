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
uint8_t *allocExtract(uint8_t *head, uint32_t firstBit, uint32_t *outlen);

#endif