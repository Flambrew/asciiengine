#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "util.h"

#define PNG_HEADER

RGB *read(char *path) {

    FILE *file = fopen(path, "r");

    uint32_t i;
    uint8_t n;
    for (i = 0; i < 16; ++i) {
        printf("%d\n", getc(file));
    }

    fclose(file);

    return NULL;
}