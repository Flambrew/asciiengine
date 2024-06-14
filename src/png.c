#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <deflate.h>
#include <chunk.h>
#include <util.h>

#define SUCCESS 0
#define INVALID_PNG_HEADER 1
#define INVALID_DATASTREAM 2
#define MISSING_HEADER_CHUNK 3
#define DISALLOWED_CHUNK 4
#define MALFORMED_CHUNK_DATA 5
#define MISORDERED_CHUNK 6
#define UNIMPLEMENTED_CRITICAL_CHUNK 7
#define INVALID_ZLIB_DATA 8

#define PNG_HEAD ((const uint8_t[8]) {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a})

int pngVerify(FILE *file) {
    uint8_t i, header[8];
    if (file == NULL || fread(header, 1, 8, file) != 8) return 0;
    for (i = 0; i < 8; ++i) 
        if (header[i] != PNG_HEAD[i]) 
            return 0;
    return 1;
}

static FILE *file;
static PCHUNK *head;
static RGB *bitmap, *palette;
static uint8_t *sigBits, *zlibDatastream;
static RGB *cleanup(int *error, int errorType, RGB *out);

RGB *allocPNG(char *path, int *error) {
    PCHUNK *curr;
    file = fopen(path, "rb");
    if (!pngVerify(file)) return cleanup(error, INVALID_PNG_HEADER, NULL);
    head = curr = allocChunkList(file);
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

    uint32_t zlibDatastreamLen;
    uint8_t i, j, flagPLTE, flagIDAT;
    for (zlibDatastreamLen = flagPLTE = flagIDAT = 0; !isType(curr, PNG_IEND); curr = curr->next) {
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

            j = zlibDatastreamLen;
            zlibDatastream = realloc(zlibDatastream, zlibDatastreamLen += curr->length);
            for (i = 0; i < curr->length; ++i) { 
                zlibDatastream[j + i] = curr->data[i];
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
    }

    printf("idat size: %d\n", zlibDatastreamLen);
    for (i = 0; i < zlibDatastreamLen; ++i)
        printf("%.2X ", zlibDatastream[i]);
    printf("\n");

    /*----====<DECOMPRESSION>====----*/
    uint16_t infoChecksum;
    infoChecksum = zlibDatastream[0] * 0x100 + zlibDatastream[1];
    printf("%d\n", infoChecksum);  
    if (infoChecksum % 31 != 0) return cleanup(error, INVALID_ZLIB_DATA, NULL); 
    printf("checksum passed\n");

    uint8_t cmethod, cinfo, fdict, flevel, *dataBlocks, checksum[4];
    cmethod = zlibDatastream[0] & 0b1111;
    cinfo = (zlibDatastream[0] >> 4) & 0b1111;
    fdict = (zlibDatastream[1] >> 5) & 0b1;
    flevel = (zlibDatastream[1] >> 6) & 0b11;
    printf("%d, %d, %d, %d\n", cmethod, cinfo, fdict, flevel);
    if (cmethod != 8 || cinfo > 7) return cleanup(error, INVALID_ZLIB_DATA, NULL);

    printf("verification passed\n");

    /*uint16_t windowSize;
    windowSize = pow2(2, cinfo + 8);

    dataBlocks = malloc(sizeof(uint8_t) * curr->length - 6);
    for (j = 0, i = 2; i < curr->length - 4; ++j, ++i) {
        dataBlocks[j] = curr->data[i];
    }

    for (j = 0; i < curr->length; ++j, ++i) {
        checksum[j] = curr->data[i];
    }*/

    //TODO IMPLEMENT




    return cleanup(error, SUCCESS, bitmap);
}

static RGB *cleanup(int *error, int errorType, RGB *out) {
    if (file != NULL) fclose(file);
    if (head != NULL) freeChunkList(head);
    if (bitmap != NULL && errorType) free(bitmap);
    if (palette != NULL) free(palette);
    if (sigBits != NULL) free(sigBits);
    if (zlibDatastream != NULL) free(zlibDatastream);
    *error = errorType;
    return out;
}

void freePNG(RGB *png) {
    free(png);
}