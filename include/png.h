#include "util.h"

#ifndef PNG_H
#define PNG_H

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

int pngVerify(FILE *file);
RGB *parsePng(char *pngPath, int *error);

#endif