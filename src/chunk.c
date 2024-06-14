#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define PNG_IHDR ((const uint8_t[4]) {0x49, 0x48, 0x44, 0x52})
#define PNG_PLTE ((const uint8_t[4]) {0x50, 0x4C, 0x54, 0x45})
#define PNG_IDAT ((const uint8_t[4]) {0x49, 0x44, 0x41, 0x54})
#define PNG_IEND ((const uint8_t[4]) {0x49, 0x45, 0x4E, 0x44})
#define PNG_sBIT ((const uint8_t[4]) {0x73, 0x42, 0x49, 0x54})
#define PNG_acTL ((const uint8_t[4]) {0x61, 0x63, 0x54, 0x4C})
#define PNG_fcTL ((const uint8_t[4]) {0x66, 0x63, 0x54, 0x4C})
#define PNG_fdAT ((const uint8_t[4]) {0x66, 0x64, 0x41, 0x54})

typedef struct pngChunk {
    uint32_t length;
    uint8_t type[4];
    uint8_t *data;
    uint32_t crc;
    struct pngChunk *next;
} PCHUNK;

int isType(PCHUNK *chunk, const uint8_t type[4]) {
    uint8_t i;
    if (chunk == NULL) return 0;
    for (i = 0; i < 4; ++i) 
        if (chunk->type[i] != type[i])
            return 0;
    return 1;
}

PCHUNK *allocChunkList(FILE *file) {
    if (file == NULL) return NULL;

    PCHUNK *out;
    out = malloc(sizeof(PCHUNK));

    uint8_t i, byte[1];
    for (i = out->length = 0; i < 4; ++i) {
        out->length <<= 8;
        if (fread(byte, sizeof(uint8_t), 1, file) != 1) return NULL;
        out->length |= *byte;
    }

    if (fread(out->type, sizeof(uint8_t), 4, file) != 4) return NULL;

    out->data = malloc(sizeof(uint8_t) * out->length);
    if (fread(out->data, sizeof(uint8_t), out->length, file) != out->length) return NULL;

    for (i = out->crc = 0; i < 4; ++i) {
        out->crc <<= 8;
        if (fread(byte, sizeof(uint8_t), 1, file) != 1) return NULL;
        out->crc |= *byte;
    }

    if (!isType(out, PNG_IEND)) {
        out->next = allocChunkList(file);
    } else {
        out->next = NULL;
    }
    
    return out;
}

void freeChunkList(PCHUNK *head) {
    if (head == NULL) return;
    free(head->data);
    freeChunkList(head->next);
}