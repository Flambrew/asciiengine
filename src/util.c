#include <stdlib.h>
#include <stdint.h>

typedef struct pixel {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} RGB;

int htoi(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

int pow2(int x, int n) {
    if (n == 1) return x;
    return x * pow2(x, n - 1);
}