#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrix.h"

int matrix_rows, matrix_cols, **matrix;

void multiplyMatrices();

int main (int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <rows> <cols> <outputfile>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Allocate output matrix in memory;
    matrix_rows = strtol(argv[1], NULL, 10);
    matrix_cols = strtol(argv[2], NULL, 10);
    if (matrix_rows == -1 || matrix_cols == -1) {
        fprintf(stderr, "Error when parsing inputs from command line.\n");
        exit(EXIT_FAILURE);
    }

    matrix = (int**) malloc(sizeof(int*) * matrix_rows);
    int i;
    for(i=0; i<matrix_rows; i++) {
        matrix[i] = (int*) malloc(sizeof(int) * matrix_cols);
    }

    srand(time(NULL));
    int j;
    for(i=0; i<matrix_rows; i++) {
        for(j=0; j<matrix_cols; j++) {
            matrix[i][j] = rand() % 1000;
        }
    }

    writeMatrix(argv[3], matrix, matrix_rows, matrix_cols);

    for (i=0; i<matrix_rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
    return 0;
}
