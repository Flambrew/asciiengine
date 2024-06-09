#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include <util.h>

#define SUCCESS 0
#define INVALID_PNG_HEADER 1
#define INVALID_DATASTREAM 2
#define MISSING_HEADER_CHUNK 3
#define DISALLOWED_CHUNK 4
#define MALFORMED_CHUNK_DATA 5
#define INVALID_ZLIB_DATA 6
#define MISSING_CHUNK 7
#define MISORDERED_CHUNK 8
#define UNIMPLEMENTED_CRITICAL_CHUNK 9

#define PNG_HEAD ((const uint8_t[8]) {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a})
#define PNG_IHDR ((const uint8_t[4]) {0x49, 0x48, 0x44, 0x52})
#define PNG_PLTE ((const uint8_t[4]) {0x50, 0x4C, 0x54, 0x45})
#define PNG_IDAT ((const uint8_t[4]) {0x49, 0x44, 0x41, 0x54})
#define PNG_IEND ((const uint8_t[4]) {0x49, 0x45, 0x4E, 0x44})
#define PNG_sBIT ((const uint8_t[4]) {0x73, 0x42, 0x49, 0x54})
#define PNG_acTL ((const uint8_t[4]) {0x61, 0x63, 0x54, 0x4C})
#define PNG_fcTL ((const uint8_t[4]) {0x66, 0x63, 0x54, 0x4C})
#define PNG_fdAT ((const uint8_t[4]) {0x66, 0x64, 0x41, 0x54})

static int pngVerify(FILE *file) {
    uint8_t i, header[8];
    if (file == NULL || fread(header, 1, 8, file) != 8) return 0;
    for (i = 0; i < 8; ++i) 
        if (header[i] != PNG_HEAD[i]) 
            return 0;
    return 1;
}

typedef struct Chunk {
    uint32_t length;
    uint8_t type[4];
    uint8_t *data;
    uint32_t crc;
    struct Chunk *next;
} Chunk;

static void freeChunk(Chunk *c) {
    if (c == NULL) return;
    free(c->data);
    freeChunk(c->next);
}

static int isType(Chunk *chunk, const uint8_t type[4]) {
    uint8_t i;
    if (chunk == NULL) return 0;
    for (i = 0; i < 4; ++i) 
        if (chunk->type[i] != type[i])
            return 0;
    return 1;
}

static Chunk *chunkList(FILE *file) {
    if (file == NULL) return NULL;

    Chunk *out = malloc(sizeof(Chunk));

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
        out->next = chunkList(file);
    } else {
        out-> next = NULL;
    }
    
    return out;
}

uint32_t *huffmanTree(uint8_t *lengths, uint32_t len) {
    uint8_t maxlen;
    uint32_t i, *bitLenCount, code, *nextCode, *out;
    for (i = 0; i < len; ++i) {
        if (maxlen < lengths[i]) {
            maxlen = lengths[i];
        }
    }

    malloc(bitLenCount, (sizeof(uint32_t) + 1) * maxlen);
    malloc(nextCode, (sizeof(uint32_t) + 1) * maxlen);
    for (i = 1; i <= maxlen; ++i) {
        bitLenCount[lengths[i]]++;
    }

    for(i = code = bitLenCount[0] = 0; i < maxlen; ++i) {
        nextCode[i] = code = (code + bitLenCount[i - 1]) << 1;
    }
}

static FILE *file;
static Chunk *head;
static RGB *bitmap, *palette;
static uint8_t *sigBits, *bytestreamIDAT;
static RGB *cleanup(int *error, int errorType, RGB *out);

RGB *parsePng(char *path, int *error) {
    Chunk *curr;

    file = fopen(path, "rb");
    if (!pngVerify(file)) return cleanup(error, INVALID_PNG_HEADER, NULL);
    head = curr = chunkList(file);
    if (head == NULL) return cleanup(error, INVALID_DATASTREAM, NULL);

    uint32_t width, height;
    uint8_t bitDepth, colorType, interlace;
    if (isType(curr, PNG_IHDR)) {
        width = ((curr->data[0] << 8 | curr->data[1]) << 8 | curr->data[2]) << 8 | curr->data[3];
        height = ((curr->data[4] << 8 | curr->data[5]) << 8 | curr->data[6]) << 8 | curr->data[7];
        bitDepth = curr->data[8];
        colorType = curr->data[9];
        interlace = curr->data[12];
    } else return cleanup(error, MISSING_HEADER_CHUNK, NULL);
    bitmap = malloc(sizeof(RGB) * width * height);
    curr = curr->next;

    uint32_t bytestreamIDATLen;
    uint8_t i, j, flagPLTE, flagIDAT;
    for (bytestreamIDATLen = flagPLTE = flagIDAT = 0; !isType(curr, PNG_IEND); curr = curr->next) {
        if (isType(curr, PNG_PLTE)) {
            if (colorType == 0 || colorType == 4) return cleanup(error, DISALLOWED_CHUNK, NULL);
            if (curr->length % 3 != 0) return cleanup(error, MALFORMED_CHUNK_DATA, NULL);
            flagPLTE = 1;

            palette = malloc(sizeof(RGB) * curr->length / 3);
            for (i = 0; i < curr->length; i += 3) {
                palette[i / 3].red = curr->data[i];
                palette[i / 3].green = curr->data[i + 1];
                palette[i / 3].blue = curr->data[i + 2];
            }
        } else if (isType(curr, PNG_IDAT)) {
            if ((colorType == 3 && !flagPLTE) || ((colorType == 0 || colorType == 4) && flagPLTE)) return cleanup(error, DISALLOWED_CHUNK, NULL);
            if (curr->length < 6) return cleanup(error, MALFORMED_CHUNK_DATA, NULL);
            flagIDAT = 1;

            j = bytestreamIDATLen;
            bytestreamIDAT = realloc(bytestreamIDAT, bytestreamIDATLen += curr->length);
            for (i = 0; i < curr->length; ++i) { 
                bytestreamIDAT[j + i] = curr->data[i];
            }
        } else if (isType(curr, PNG_sBIT)) {
            if (flagPLTE || flagIDAT) return cleanup(error, MISORDERED_CHUNK, NULL);

            sigBits = malloc(sizeof(uint8_t) * curr->length);
            for(i = 0; i < curr->length; ++i) {
                sigBits[i] = curr->data[i];
            }
        } else if (isType(curr, PNG_acTL)) { printf("unimplemented chunk (TODO): %.4s\n", curr->type);
        } else if (isType(curr, PNG_fcTL)) { printf("unimplemented chunk (TODO): %.4s\n", curr->type);
        } else if (isType(curr, PNG_fdAT)) { printf("unimplemented chunk (TODO): %.4s\n", curr->type);
        } else if ((curr->type[0] & 0b00100000) == 0) { 
            return cleanup(error, UNIMPLEMENTED_CRITICAL_CHUNK, NULL);
        } else printf("unimplemented chunk: %.4s\n", curr->type);

        /* --- animation implementation --- //
        // --- animation implementation --- */
    }

    printf("idat size: %d\n", bytestreamIDATLen);
    for (i = 0; i < bytestreamIDATLen; ++i)
        printf("%.2X ", bytestreamIDAT[i]);
    printf("\n");

    /*----====<DECOMPRESSION>====----*/
    
    uint16_t infoChecksum;
    infoChecksum = curr->data[0] * 0xFF + curr->data[1];
    if (infoChecksum % 31 != 0) return cleanup(error, INVALID_ZLIB_DATA, NULL);

    uint8_t cmethod, cinfo, fdict, flevel, *dataBlocks, checksum[4];
    cmethod = curr->data[0] & 0b00001111;
    cinfo = (curr->data[0] & 0b11110000) >> 4;
    fdict = (curr->data[1] & 0b00100000) >> 5;
    flevel = (curr->data[1] & 0b11000000) >> 6;
    if (cmethod != 8 || cinfo > 7) return cleanup(error, INVALID_ZLIB_DATA, NULL);

    uint16_t windowSize;
    windowSize = pow2(2, cinfo + 8);

    dataBlocks = malloc(sizeof(uint8_t) * curr->length - 6);
    for (j = 0, i = 2; i < curr->length - 4; ++j, ++i) {
        dataBlocks[j] = curr->data[i];
    }

    for (j = 0; i < curr->length; ++j, ++i) {
        checksum[j] = curr->data[i];
    }

    //TODO IMPLEMENT




    return cleanup(error, SUCCESS, bitmap);
}

static RGB *cleanup(int *error, int errorType, RGB *out) {
    if (file != NULL) fclose(file);
    if (head != NULL) freeChunk(head);
    if (bitmap != NULL && errorType) free(bitmap);
    if (palette != NULL) free(palette);
    if (sigBits != NULL) free(sigBits);
    if (bytestreamIDAT != NULL) free(bytestreamIDAT);
    *error = errorType;
    return out;
}