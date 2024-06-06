#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "png.h"
#include "pixASCII.h"
#include "util.h"

#define MIN(x, y) (((x) < (y) ? (x) : (y)))
#define MAX(x, y) (((x) > (y) ? (x) : (y)))

int main() {
    char **shadeMap;
    RGB rgb = {0, 0, 0};

    shadeMap = getShadeMap("asset/shades.bin");
    getAscii(&rgb, shadeMap);
    freeStrArr(shadeMap, SHADERANGE);

    int error;
    parsePng("asset\\test.png", &error);

    printf("finished with error code: %d\n", error);
}

/*int main() { // prints a gradient with 0.0625% noise
    int i, r, n;
    char **shadeMap;
    RGB rgb;

    srand(time(NULL));
    shadeMap = getShadeMap("asset/shades.bin");
    for (i = 0; i < SHADERANGE * 4; ++i) {
        r = (rand() % 33 - 16);
        n = MAX(MIN((i / 4) + r, SHADERANGE - 1), 0);
        rgb.red = n;
        rgb.green = n;
        rgb.blue = n;
        printf("%c", getAscii(&rgb, shadeMap));
        if ((i + 1) % (SHADERANGE / 4) == 0) {
            printf("\n");
        }
    }

    freeStrArr(shadeMap, SHADERANGE);

    return 0;
}*/