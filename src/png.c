#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "util.h"

bool pngVerify(FILE *file) {
    int i, PNG_HEADER[] = {137, 80, 78, 71, 13, 10, 26, 10};
    for (i = 0; i < 8; ++i) {
        if (PNG_HEADER[i] != getc(file)) {
            return false;
        }
    }
    return true;
}

RGB *read(char *path) {
    FILE *file;
    RGB *bitmap;

    file = fopen(path, "r");

    if (!pngVerify(file)) return NULL;

    fclose(file);
    return NULL;
}