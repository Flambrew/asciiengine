#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "ascii.h"
#include "util.h"
#include "png.h"

#define MIN(x, y) (((x) < (y) ? (x) : (y)))
#define MAX(x, y) (((x) > (y) ? (x) : (y)))

#if defined(_WIN32) || defined(_WIN64)
    #define PATH_SEPARATOR '\\'
#else
    #define PATH_SEPARATOR '/'
#endif

int main() {

    printf("hello world\n");

    char shadeMapPath[256], imgPath[256];
    char **shadeMap;
    RGB rgb = {0, 0, 0};

    sprintf(shadeMapPath, "assets%c%s", PATH_SEPARATOR, "shades.bin");
    shadeMap = getShadeMap(shadeMapPath);
    getAscii(&rgb, shadeMap);
    freeShadeMap(shadeMap);

    int error;
    sprintf(imgPath, "assets%c%s", PATH_SEPARATOR, "test2.png");
    parsePng(imgPath, &error);

    printf("finished with error code: %d\n", error);
    return 0;
}