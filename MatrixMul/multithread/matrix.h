#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>


int* readMatrix(const char *filename, int *rows, int *cols);
int matchIdentifier (FILE *f, const char *identifier);
int writeMatrix(const char *filename, int *saida, int rows, int cols);


#endif
