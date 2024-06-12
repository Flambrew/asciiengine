#include <stdlib.h>
#include <stdint.h>

typedef struct pixel {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} RGB;

typedef struct huffmanNode {
    char symbol;
    uint8_t length;
    uint32_t code;
} HNODE;

int htoi(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

void freeStrArr(char **arr, int len) {
    int i;
    for (i = 0; i < len; ++i) {
        if (arr[i] != NULL) {
            free(arr[i]);
        }
    }
    free(arr);
}

int pow2(int x, int n) {
    if (n == 1) return x;
    return x * pow2(x, n - 1);
}

void huffmanTree(HNODE *tree, uint32_t len) {
    uint32_t i, maxlen, *bitLenCounts, code, *next_code;
    for (maxlen = i = 1; i <= len; ++i) 
        if (maxlen < tree[i].length) 
            maxlen = tree[i].length;
    bitLenCounts = calloc(maxlen + 1, (sizeof(uint32_t)));
    next_code = malloc(len * sizeof(uint32_t));

    for (i = 1; i <= len; ++i) 
        bitLenCounts[tree[i].length]++;
    for(i = code = bitLenCounts[0] = 0; i < maxlen; ++i) 
        next_code[i + 1] = code = (code + bitLenCounts[i]) << 1;
    for (i = 0; i <= len; ++i) {
        int thislen = tree[i].length;
        if (thislen != 0) {
            tree[i].code = next_code[thislen];
            next_code[thislen]++;
        }
    }

    free(bitLenCounts);
    free(next_code);
}