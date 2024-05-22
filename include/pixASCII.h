#include "util.h"

#ifndef PIX_ASCII_H
#define PIX_ASCII_H

#define MAXCHARS 10
#define SHADERANGE 256

char **getShadeMap(char *shadeBinPath);
char getAscii(RGB rgb, char **shadeMap);

#endif