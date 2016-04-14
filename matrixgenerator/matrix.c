#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"

int writeMatrix(const char *filename, int **matrix, int rows, int cols) {
    FILE *f;

    f = fopen(filename, "w");
    if (!f) {
        fprintf(stderr, "Error while creating output file.\n");
        exit(EXIT_FAILURE);
    }
    fprintf(f, "LINHAS = %d\n", rows);
    fprintf(f, "COLUNAS = %d\n", cols);
    int i, j;
    for (i=0; i<rows; i++) {
        for (j=0; j<cols; j++) {
            fprintf(f, "%d ", matrix[i][j]);
        }
        fprintf(f, "\n");
    }

    fclose(f);

    return 1;
}
