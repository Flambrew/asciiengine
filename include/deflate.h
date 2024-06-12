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

typedef struct dataChunk {
    uint8_t final, type;
    uint32_t len, nlen;
    uint8_t *bitstream;
} DCHUNK;

DCHUNK *allocBlockList();

void freeBlockList(DCHUNK *head);

#endif