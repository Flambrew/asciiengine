#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include <util.h>

// --- header & chunk identification data --- //
const uint8_t PNG_HEADER[8] = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a};
const uint8_t PNG_IHDR[4] = {0x49, 0x48, 0x44, 0x52}, PNG_PLTE[4] = {0x50, 0x4C, 0x54, 0x45}, PNG_IDAT[4] = {0x49, 0x44, 0x41, 0x54};
const uint8_t PNG_IEND[4] = {0x49, 0x45, 0x4E, 0x44}, PNG_sBIT[4] = {0x73, 0x42, 0x49, 0x54}, PNG_pHYs[4] = {0x70, 0x48, 0x59, 0x73};
const uint8_t PNG_acTL[4] = {0x61, 0x63, 0x54, 0x4C}, PNG_fcTL[4] = {0x66, 0x63, 0x54, 0x4C}, PNG_fdAT[4] = {0x66, 0x64, 0x41, 0x54};

/* --- unimplemented chunks --- //
const uint8_t PNG_tRNS[4] = {0x74, 0x52, 0x4E, 0x53}, PNG_cHRM[4] = {0x63, 0x48, 0x52, 0x4D}, PNG_gAMA[4] = {0x67, 0x41, 0x4D, 0x41};
const uint8_t PNG_iCCP[4] = {0x69, 0x43, 0x43, 0x50}, PNG_sRGB[4] = {0x73, 0x52, 0x47, 0x42}, PNG_cICP[4] = {0x63, 0x49, 0x43, 0x50};
const uint8_t PNG_mDCv[4] = {0x6D, 0x44, 0x43, 0x76}, PNG_iTXt[4] = {0x69, 0x54, 0x58, 0x74}, PNG_tEXt[4] = {0x74, 0x45, 0x58, 0x74};
const uint8_t PNG_zTXt[4] = {0x7A, 0x54, 0x58, 0x74}, PNG_bKGD[4] = {0x62, 0x4B, 0x47, 0x44}, PNG_hIST[4] = {0x68, 0x49, 0x53, 0x54};
const uint8_t PNG_sPLT[4] = {0x73, 0x50, 0x4C, 0x54}, PNG_eXlf[4] = {0x65, 0x58, 0x6C, 0x66}, PNG_tIME[4] = {0x74, 0x49, 0x4D, 0x45};
// --- unimplemented chunks --- */

typedef struct Chunk {
    uint32_t length;
    uint8_t type[4];
    uint8_t *data;
    uint32_t crc;
    Chunk *next;
} Chunk;

static int pngVerify(FILE *file) {
    uint8_t i;
    for (i = 0; i < 8; ++i) {
        if (getc(file) != PNG_HEADER[i]) {
            return 0;
        }
    }
    return 1;
}

static int isType(Chunk *chunk, uint8_t type[4]) {
    uint8_t i;
    for (i = 0; i < 4; ++i) {
        if (chunk->type[i] != type[i]) {
            return 0;
        }
    }
    return 1;
}

static Chunk *nextChunk(FILE *file) {
    Chunk *out;

    uint8_t i;
    for (i = 0; i < 4; ++i) {
        out->length <<= 8;
        out->length |= getc(file);
    }

    for (i = 0; i < 4; ++i) {
        out->type[i] = getc(file);
    }

    out->data = malloc(sizeof(uint8_t) * out->length);
    for (i = 0; i < out->length; ++i) {
        out->data[i] = getc(file);
    }

    for (i = 0; i < 4; ++i) {
        out->crc = getc(file);
    }

    if (!isType(out, PNG_IEND)) {
        out->next = nextChunk(file);
    } else {
        out-> next = NULL;
    }
    
    return out;
}

static void freeChunk(Chunk *c) {
    if (c == NULL) return;
    free(c->data);
    freeChunk(c->next);
}

RGB *parsePng(char *path) {
    FILE *file;
    RGB *bitmap;
    Chunk *curr;

    file = fopen(path, "r");
    if (!pngVerify(file)) return NULL;
    curr = nextChunk(file);
    fclose(file);

    uint32_t width, height;
    uint8_t bitDepth, colorType, interlace; // TODO: implement interlace

    if (isType(curr, PNG_IHDR)) {
        width = ((curr->data[0] << 8 | curr->data[1]) << 8 | curr->data[2]) << 8 | curr->data[3];
        height = ((curr->data[4] << 8 | curr->data[5]) << 8 | curr->data[6]) << 8 | curr->data[7];
        bitDepth = curr->data[8];
        colorType = curr->data[9];
        interlace = curr->data[12];
    } else {
        return NULL;
    }

    bitmap = malloc(sizeof(RGB) * width * height);

    for (curr = curr->next; isType(curr, PNG_IEND); curr = curr->next) {
        if (isType(curr, PNG_PLTE)) {

        } else if (isType(curr, PNG_IDAT)) {

        } else if (isType(curr, PNG_sBIT)) {

        } else if (isType(curr, PNG_pHYs)) {

        } else if (isType(curr, PNG_acTL)) {

        } else if (isType(curr, PNG_fcTL)) {

        } else if (isType(curr, PNG_fdAT)) {

        } else {
            
        }
    }

    return bitmap;
}