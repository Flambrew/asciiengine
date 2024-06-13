#include <stdint.h>
#include <stdlib.h>

typedef struct huffmanNode {
    char symbol;
    uint8_t length;
    uint32_t code;
} HNODE;
void huffmanTree(HNODE *tree, uint32_t len) {
    uint32_t i, maxlen, *bitLenCounts, code, *next_code;
    for (maxlen = i = 1; i <= len; ++i) 
        if (maxlen < tree[i].length) 
            maxlen = tree[i].length;
    bitLenCounts = calloc(maxlen + 1, (sizeof(uint32_t)));
    next_code = malloc(len * sizeof(uint32_t));

    for (i = 1; i <= len; ++i) 
        bitLenCounts[tree[i].length]++;
    for(i = code = bitLenCounts[0] = 0; i < maxlen; ++i) 
        next_code[i + 1] = code = (code + bitLenCounts[i]) << 1;
    for (i = 0; i <= len; ++i) {
        int thislen = tree[i].length;
        if (thislen != 0) {
            tree[i].code = next_code[thislen];
            next_code[thislen]++;
        }
    }

    free(bitLenCounts);
    free(next_code);
}

typedef struct dataChunk {
    uint8_t final, type;
    uint32_t len, nlen;
    uint8_t *bitstream;
    struct dataChunk *nextChunk;
} DCHUNK;

static uint8_t *getBits(uint8_t *datastream, uint32_t bitIndex, uint32_t quantity) {
    if (quantity == 0) return NULL;

    uint8_t *out;
    out = malloc((quantity + 7) / 8 * sizeof(uint8_t));

    // finish
}

DCHUNK *allocBlockList(uint8_t *datastream, uint32_t bitIndex) {
    DCHUNK *curr;

    curr->final = (datastream[bitIndex / 8] >> (bitIndex % 8)) & 0b1; bitIndex += 1;

    curr->type = (datastream[bitIndex / 8] >> (bitIndex % 8)) & 0b11; bitIndex += 2;


    if (!curr->final) {
        curr->nextChunk = allocBlockList(datastream, bitIndex);
    }
    
    return curr;
}

void freeChunkList(DCHUNK *head);