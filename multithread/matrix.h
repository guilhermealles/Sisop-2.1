#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>


matrix* readMatrix(const char *filenameint *rows, int *cols);
int matchIdentifier (FILE *f, const char *identifier);
void printMatrix(int **matrix, int rows, int cols);
void errorExit (const char *error_msg);
int writeMatrix(const char *filename, matrix *saida);


#endif
