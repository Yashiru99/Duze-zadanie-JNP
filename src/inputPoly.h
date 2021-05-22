#ifndef POLY_INPUTPOLY_H
#define POLY_INPUTPOLY_H
#include <stdio.h>
typedef struct{
    size_t length;
    char *letters;
}line;

typedef struct{
    size_t length;
    line *Polys;
}lines;

void ReadFile();
#endif //POLY_INPUTPOLY_H
