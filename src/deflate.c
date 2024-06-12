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
} DCHUNK;

DCHUNK *allocBlockList();

void freeChunkList(DCHUNK *head);