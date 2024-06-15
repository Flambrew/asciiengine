#include <stdint.h>
#include <stdlib.h>
#include <stdio.h> // REMOVE

typedef struct huffmanNode {
    char symbol;
    uint8_t length;
    uint32_t code;
} HNODE;

void allocHuffmanTree(HNODE *tree, uint32_t len) {
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

static uint32_t index = 0;
static uint8_t *stream = NULL;

static uint8_t nextBit() {
    uint8_t bit;
    bit = (stream[index / 8] >> (index % 8)) & 0b1;
    ++index;
    return bit;
}

static uint8_t nextByte() {
    uint8_t i, out;
    for (i = out = 0; i < 8; ++i) 
        out = (out >> 1) + (nextBit() << 7);
    return out;
}

static void endByte() {
    while (index % 8 != 0) {
        ++index;
    }
}

// STORE THE LEN IN THE FIRST 4 BYTES
uint8_t *allocDeflate(uint8_t *datastream) {
    stream = datastream;
    index = 0;

    uint8_t *outstream;
    outstream = malloc(4 * sizeof(uint8_t));

    uint32_t *outlen;
    outlen = (uint32_t *) outstream;
    *outlen = 32;

    uint8_t i, final, type, len;
    do {
        final = nextBit();
        type = (nextBit() << 1) + nextBit();

        if (type == 0b00) {
            printf("nocompress\n");
            endByte();
            printf("\tendbyte\n");
            len = nextByte() * 256 + nextByte();
            printf("\tlen: %d\n", len);
            int nlen = (nextByte() * 256 + nextByte());
            if ((len ^ 0xFFFF) != nlen) {
                return NULL;
            }
            printf("\tnlen passed\n");

            outstream = realloc(outstream, (*outlen + len + 7) / 8);
            for (i = *outlen; i < *outlen + len; ++i) 
                outstream[i / 8] |= (nextBit() << (i % 8));
            *outlen += len;

        } else if (type != 0b11) {
            printf("compress\n");
            if (type == 0b10) {
                printf("\tdynamic\n");
                // READ REPRESENTATION OF CODE TREES
            }
            i = 0;
            while (i++ < 150) {
                len = 0; // DECODE LITERAL/LENGTH VALUE
                if (len < 256) {
                    
                } else if (len == 256) {
                    break;
                } else {

                }
            }
        } else {
            printf("ERRORVAL\n");
            return NULL;
        }


    } while (!final);
    
    printf("\n");
    return outstream;
}

// 0100000000000000000000000000000010010010010010001101101001101110