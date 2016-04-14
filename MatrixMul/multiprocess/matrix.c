#include <stdlib.h>
#include "matrix.h"

int* readMatrix (const char *filename, int *rows, int *cols) {
    FILE *f;
    int local_rows=0, local_cols=0;

    f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Fatal error: failed to open file.\n");
        exit(EXIT_FAILURE);
    }

    if (matchIdentifier(f, "LINHAS =")) {
        fscanf(f, "%u\n", &local_rows);
    }
    else {
        fprintf(stderr, "Error: expected \"LINHAS =\".\n");
        exit(EXIT_FAILURE);
    }

    if (matchIdentifier(f, "COLUNAS =")) {
        fscanf(f, "%u\n", &local_cols);
    }
    else {
        fprintf(stderr, "Error: expected \"COLUNAS =\".\n");
        exit(EXIT_FAILURE);
    }

    // Allocate matrix in memory
    int *matrix;
    matrix = (int*) malloc(sizeof(int) * local_rows * local_cols);
    int i, j;
    for (i=0; i<local_rows; i++) {
        for (j=0; j<local_cols; j++) {
            int value;
            if (fscanf(f, "%d ", &value) == 1) {
                matrix[(i*local_rows) + j] = value;
            }
            else {
                fprintf(stderr, "Error reading value from matrix file.\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    *rows = local_rows;
    *cols = local_cols;
    fclose(f);
    return matrix;
}

int writeMatrix(const char *filename, int *matrix, int rows, int cols) {
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
            fprintf(f, "%d ", matrix[(i*rows)+j]);
        }
        fprintf(f, "\n");
    }

    fclose(f);

    return 1;
}

int matchIdentifier (FILE *f, const char *identifier) {
    int i=0, done=0;
    char c;

    while (! done) {
        if (identifier[i] == '\0') {
            done = 1;
        }
        else {
            c = fgetc(f);
            if (identifier[i] != c) {
                return 0;
            }
            i++;
        }
    }
    return 1;
}
