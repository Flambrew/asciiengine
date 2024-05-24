#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "util.h"

#define PNG_HEADER 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a

typedef struct Chunk {
    uint32_t length;
    uint8_t type[4];
    uint8_t *data;
    uint32_t crc;
} Chunk;

static bool pngVerify(FILE *file) {
    uint8_t i, header[8] = {PNG_HEADER};
    for (i = 0; i < 8; ++i) {
        if (getc(file) != header[i]) {
            return false;
        }
    }
    return true;
}

static Chunk nextChunk(FILE *file) {
    Chunk out = {0, 0, 0, 0};

    uint8_t i;
    for (i = 0; i < 4; ++i) {
        out.length <<= 8;
        out.length |= getc(file);
    }

    for (i = 0; i < 4; ++i) {
        out.type[i] = getc(file);
    }

    out.data = malloc(sizeof(uint8_t) * out.length);
    for (i = 0; i < out.length; ++i) {
        out.data[i] = getc(file);
    }

    for (i = 0; i < 4; ++i) {
        out.crc = getc(file);
    }
    
    return out;
}

static void freeChunk(Chunk c) {
    free(c.data);
}

RGB *read(char *path) {
    FILE *file;
    RGB *bitmap;

    file = fopen(path, "r");

    if (!pngVerify(file)) return NULL;

    fclose(file);
    return NULL;
}