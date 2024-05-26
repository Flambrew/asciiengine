#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include <util.h>

#define SUCCESS 0
#define INVALID_PNG_HEADER 1
#define INVALID_DATASTREAM 2
#define MISSING_HEADER_CHUNK 3
#define DISALLOWED_CHUNK 4
#define INVALID_CHUNK_DATA 5
#define MISSING_CHUNK 6
#define MISORDERED_CHUNK 7
#define UNIMPLEMENTED_CHUNK 8

// --- header & chunk identification data --- //
#define PNG_HEADER ((const uint8_t[8]) {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a})
#define PNG_IHDR ((const uint8_t[4]){0x49, 0x48, 0x44, 0x52})
#define PNG_PLTE ((const uint8_t[4]){0x50, 0x4C, 0x54, 0x45})
#define PNG_IDAT ((const uint8_t[4]){0x49, 0x44, 0x41, 0x54})
#define PNG_IEND ((const uint8_t[4]){0x49, 0x45, 0x4E, 0x44})
#define PNG_sBIT ((const uint8_t[4]){0x73, 0x42, 0x49, 0x54})
#define PNG_pHYs ((const uint8_t[4]){0x70, 0x48, 0x59, 0x73})
// #define PNG_acTL ((const uint8_t[4]){0x61, 0x63, 0x54, 0x4C}) // TODO impl
// #define PNG_fcTL ((const uint8_t[4]){0x66, 0x63, 0x54, 0x4C}) // TODO impl
// #define PNG_fdAT ((const uint8_t[4]){0x66, 0x64, 0x41, 0x54}) // TODO impl

/* --- unimplemented chunks --- //
#define PNG_tRNS ((const uint8_t[4]){0x74, 0x52, 0x4E, 0x53})
#define PNG_cHRM ((const uint8_t[4]){0x63, 0x48, 0x52, 0x4D})
#define PNG_gAMA ((const uint8_t[4]){0x67, 0x41, 0x4D, 0x41})
#define PNG_iCCP ((const uint8_t[4]){0x69, 0x43, 0x43, 0x50})
#define PNG_sRGB ((const uint8_t[4]){0x73, 0x52, 0x47, 0x42})
#define PNG_cICP ((const uint8_t[4]){0x63, 0x49, 0x43, 0x50})
#define PNG_mDCv ((const uint8_t[4]){0x6D, 0x44, 0x43, 0x76})
#define PNG_iTXt ((const uint8_t[4]){0x69, 0x54, 0x58, 0x74})
#define PNG_tEXt ((const uint8_t[4]){0x74, 0x45, 0x58, 0x74})
#define PNG_zTXt ((const uint8_t[4]){0x7A, 0x54, 0x58, 0x74})
#define PNG_bKGD ((const uint8_t[4]){0x62, 0x4B, 0x47, 0x44})
#define PNG_hIST ((const uint8_t[4]){0x68, 0x49, 0x53, 0x54})
#define PNG_sPLT ((const uint8_t[4]){0x73, 0x50, 0x4C, 0x54})
#define PNG_eXlf ((const uint8_t[4]){0x65, 0x58, 0x6C, 0x66})
#define PNG_tIME ((const uint8_t[4]){0x74, 0x49, 0x4D, 0x45})
// --- unimplemented chunks --- */

typedef struct Chunk {
    uint32_t length;
    uint8_t type[4];
    uint8_t *data;
    uint32_t crc;
    struct Chunk *next;
} Chunk;

static int pngVerify(FILE *file) {
    uint8_t i;
    if (file == NULL) return 0;
    for (i = 0; i < 8; ++i) {
        if (getc(file) != PNG_HEADER[i]) {
            return 0;
        }
    }
    return 1;
}

static int isType(Chunk *chunk, const uint8_t type[4]) {
    uint8_t i;
    if (chunk == NULL) return 0;
    for (i = 0; i < 4; ++i) {
        if (chunk->type[i] != type[i]) {
            return 0;
        }
    }
    return 1;
}

static Chunk *chunkList(FILE *file) {
    if (file == NULL) return NULL;

    Chunk *out = malloc(sizeof(Chunk));

    uint8_t i;
    for (i = out->length = 0; i < 4; ++i) {
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

    for (i = out->crc = 0; i < 4; ++i) {
        out->crc <<= 8;
        out->crc |= getc(file);
    }

    if (!isType(out, PNG_IEND)) {
        out->next = chunkList(file);
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

static FILE *file;
static Chunk *head;
static RGB *bitmap, *palette;
static uint8_t *sigBits;

static RGB *cleanup(int *error, int errorType, RGB *out) {
    if (file != NULL) fclose(file);
    if (head != NULL) freeChunk(head);
    if (bitmap != NULL && errorType) free(bitmap);
    if (palette != NULL) free(palette);
    if (sigBits != NULL) free(sigBits);
    *error = errorType;
    return NULL;
}

RGB *parsePng(char *path, int *error) {
    Chunk *curr;
    file = fopen(path, "r");
    if (!pngVerify(file)) return cleanup(error, INVALID_PNG_HEADER, NULL);
    head = curr = chunkList(file);
    if (head == NULL) return cleanup(error, INVALID_DATASTREAM, NULL);

    uint32_t width, height;
    uint8_t bitDepth, colorType, interlace; // TODO: implement interlace
    if (isType(curr, PNG_IHDR)) {
        width = ((curr->data[0] << 8 | curr->data[1]) << 8 | curr->data[2]) << 8 | curr->data[3];
        height = ((curr->data[4] << 8 | curr->data[5]) << 8 | curr->data[6]) << 8 | curr->data[7];
        bitDepth = curr->data[8];
        colorType = curr->data[9];
        interlace = curr->data[12];
    } else return cleanup(error, MISSING_HEADER_CHUNK, NULL);
    bitmap = malloc(sizeof(RGB) * width * height);
    curr = curr->next;

    uint8_t i, flagPLTE, flagIDAT;
    for (flagPLTE = flagIDAT = 0; isType(curr, PNG_IEND); curr = curr->next) {
        if (isType(curr, PNG_PLTE)) {
            if (colorType == 0 || colorType == 4) return cleanup(error, DISALLOWED_CHUNK, NULL);
            if (curr->length % 3 != 0) return cleanup(error, INVALID_CHUNK_DATA, NULL);
            flagPLTE = 1;

            palette = malloc(sizeof(RGB) * curr->length / 3);
            for (i = 0; i < curr->length; i += 3) {
                palette[i / 3].red = curr->data[i];
                palette[i / 3].green = curr->data[i + 1];
                palette[i / 3].blue = curr->data[i + 2];
            }
        } else if (isType(curr, PNG_IDAT)) {
            if (colorType == 0 || colorType == 4 || !flagPLTE) return cleanup(error, DISALLOWED_CHUNK, NULL);
            flagIDAT = 1;





        } else if (isType(curr, PNG_sBIT)) {
            if (flagPLTE || flagIDAT) return cleanup(error, MISORDERED_CHUNK, NULL);

            sigBits = malloc(sizeof(uint8_t) * curr->length);
            for(i = 0; i < curr->length; ++i) {
                sigBits[i] = curr->data[i];
            }
        } else if (isType(curr, PNG_pHYs)) {





        } else return cleanup(error, UNIMPLEMENTED_CHUNK, NULL);

        /* --- animation implementation --- //
        } else if (isType(curr, PNG_acTL)) {
        } else if (isType(curr, PNG_fcTL)) {
        } else if (isType(curr, PNG_fdAT)) {
        // --- animation implementation --- */
    }

    return cleanup(error, SUCCESS, bitmap);
}

