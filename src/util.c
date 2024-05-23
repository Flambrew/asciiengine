#include <stdlib.h>
#include <stdint.h>

typedef struct RGB {
    uint8_t red, green, blue;
} RGB;

uint8_t htoi(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

void freeStrArr(char **arr, uint32_t len) {
    uint32_t i;
    for (i = 0; i < len; ++i) {
        if (arr[i] != NULL) {
            free(arr[i]);
        }
    }
    free(arr);
}

uint32_t bigEndianBlock(int *bytes) {
    uint32_t out, i;
    for (i = out = 0; i < 4; ++i) {
        out = (out << 8) | bytes[i];
    }
    return out;
}