#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"

int matrix1_rows, matrix1_cols, *matrix1;
int matrix2_rows, matrix2_cols, *matrix2;
int matrix_out_rows, matrix_out_cols, *matrix_out;

void multiplyMatrices();

int main (int argc, char **argv) {
    matrix1 = readMatrix(argv[1], &matrix1_rows, &matrix1_cols);
    matrix2 = readMatrix(argv[2], &matrix2_rows, &matrix2_cols);

    if (matrix1_cols != matrix2_rows) {
        fprintf(stderr, "Error: these two matrices cannot be multiplied!\n");
        exit(EXIT_FAILURE);
    }

    // Allocate output matrix in memory;
    matrix_out_rows = matrix1_rows;
    matrix_out_cols = matrix2_cols;
    matrix_out = (int*) malloc(sizeof(int) * (matrix_out_rows * matrix_out_cols));

    multiplyMatrices();
    writeMatrix("out.txt", matrix_out, matrix_out_rows, matrix_out_cols);
    return 0;
}

void multiplyMatrices() {
    int i, j, k, sum=0;

    for(i=0; i<matrix1_rows; i++) {
        for(j=0; j<matrix2_cols; j++) {
            for(k=0; k<matrix2_rows; k++) {
                sum += matrix1[(i*matrix1_rows) + k] * matrix2[(k*matrix2_rows) + j];
            }

            matrix_out[(i*matrix_out_rows) + j] = sum;
            sum = 0;
        }
    }
}
