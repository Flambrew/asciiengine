#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define PNG_IHDR ((const uint8_t[4]) {0x49, 0x48, 0x44, 0x52})
#define PNG_PLTE ((const uint8_t[4]) {0x50, 0x4C, 0x54, 0x45})
#define PNG_IDAT ((const uint8_t[4]) {0x49, 0x44, 0x41, 0x54})
#define PNG_IEND ((const uint8_t[4]) {0x49, 0x45, 0x4E, 0x44})
#define PNG_sBIT ((const uint8_t[4]) {0x73, 0x42, 0x49, 0x54})
#define PNG_acTL ((const uint8_t[4]) {0x61, 0x63, 0x54, 0x4C})
#define PNG_fcTL ((const uint8_t[4]) {0x66, 0x63, 0x54, 0x4C})
#define PNG_fdAT ((const uint8_t[4]) {0x66, 0x64, 0x41, 0x54})

#ifndef CHUNK_H
#define CHUNK_H

typedef struct Chunk {
    uint32_t length;
    uint8_t type[4];
    uint8_t *data;
    uint32_t crc;
    struct Chunk *next;
} Chunk;

int isType(Chunk *chunk, const uint8_t type[4]);
Chunk *allocChunkList(FILE *file);
void freeChunkList(Chunk *head);

#endif