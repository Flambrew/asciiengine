#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "util.h"

#define MAXCHARS 10
#define SHADERANGE 256

char **allocShadeMap(char *shadesPath) {
    char **shadeVals;
    shadeVals = malloc(sizeof(char *) * SHADERANGE);

    char c, *temp;
    FILE *shadeFile;
    int32_t i, state, index;
    shadeFile = fopen(shadesPath, "r");
    for (i = state = 0; (c = getc(shadeFile)) != EOF; ++state) {
        if (state == 0) {
            index = htoi(c);
        } else if (state == 1) {
            index = index * 16 + htoi(c);
            temp = malloc(sizeof(char) * MAXCHARS);
        } else if (state >= 2) {
            if (c == '_') {
                temp[i] = '\0';
                shadeVals[index] = temp;
                i = index = 0;
                state = -1;
            } else if (i >= MAXCHARS) {
                continue;
            } else {
                temp[i++] = c;
            }
        }
    }
    
    temp = NULL;
    for (i = 0; i < SHADERANGE; ++i) {
        if (shadeVals[i] == NULL) {
            if (temp == NULL) {
                return NULL;
            } else {
                shadeVals[i] = strncpy(malloc(sizeof(char) * MAXCHARS), temp, MAXCHARS);
            }
        } else {
            temp = shadeVals[i];
        }
    }

    fclose(shadeFile);
    return shadeVals;
}

void freeShadeMap(char **shadeMap) {
    int i;
    for (i = 0; i < SHADERANGE; ++i) {
        if (shadeMap[i] != NULL) {
            free(shadeMap[i]);
        }
    }
    free(shadeMap);
}

char getAscii(RGB *rgb, char **shadeMap) {
    int len;
    char *chars, c;

    chars = shadeMap[(rgb->red + rgb->green + rgb->blue) / 3];
    for (len = 0; (c = chars[len]) != '\0'; ++len);
    return chars[rand() % len];
}