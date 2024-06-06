#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "png.h"
#include "pixASCII.h"
#include "util.h"

#define MIN(x, y) (((x) < (y) ? (x) : (y)))
#define MAX(x, y) (((x) > (y) ? (x) : (y)))

#ifdef _WIN32
    #define PATH_SEPARATOR '\\'
    #define PATH_SEPARATOR_STRING "\\"
#else
    #define PATH_SEPARATOR '/'
    #define PATH_SEPARATOR_STRING "/"
#endif

int main() {
    char shadeMapPath[256], imgPath[256];
    char **shadeMap;
    RGB rgb = {0, 0, 0};

    sprintf(shadeMapPath, "assets%c%s", PATH_SEPARATOR, "shades.bin");
    shadeMap = getShadeMap(shadeMapPath);
    getAscii(&rgb, shadeMap);
    freeStrArr(shadeMap, SHADERANGE);

    int error;
    sprintf(imgPath, "assets%c%s", PATH_SEPARATOR, "test.png");
    parsePng(imgPath, &error);

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