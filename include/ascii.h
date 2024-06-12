#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "util.h"

#ifndef ASCII_H
#define ASCII_H

char **allocShadeMap(char *shadeBinPath);
char freeShadeMap(char **shadeMap);
char getAscii(RGB *rgb, char **shadeMap);

#endif