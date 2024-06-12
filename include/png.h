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

#ifndef PNG_H
#define PNG_H

int pngVerify(FILE *file);
RGB *parsePng(char *pngPath, int *error);

#endif