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
    return 0;
}