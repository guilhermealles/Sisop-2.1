#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "matrix.h"

int num_threads;

int matrix1Rows, matrix1Cols, *matrix1;
int matrix2Rows, matrix2Cols, *matrix2;
int multMatrixRows, multMatrixCols, *multMatrix;

void* multiply(void* tid){
	int i, j,k, result = 0;
	int line = (int) tid;

	for(k = line; k < matrix1Rows; k += num_threads){
		for(i=0; i < matrix2Cols; i++){
			for(j=0; j < matrix1Rows; j++){
				result = result + (matrix1[(k*matrix1Rows) + j] * matrix2[(j*matrix2Rows) + i]);
			}
			multMatrix[(k*multMatrixRows) + i] = result;
			result = 0;
		}
	}

	return 0;
}

int main (int argc, char **argv) {
   if (argc == 5) {
		pthread_t * thread;
		int i=0;

        // read and allocate matrix
        matrix1 = readMatrix(argv[1], &matrix1Rows, &matrix1Cols);
		matrix2 = readMatrix(argv[2], &matrix2Rows, &matrix2Cols);

		num_threads = strtol(argv[3], NULL, 10);

		if(num_threads <= 0){
			fprintf(stderr, "Threads number must be higher than 0.\n");
			exit(EXIT_FAILURE);
		}

		// validate if is possible to multiply
		if(matrix1Cols == matrix2Rows){
			multMatrixRows = matrix1Rows;
			multMatrixCols = matrix2Cols;
			multMatrix = (int*) malloc(sizeof(int) * multMatrixRows * multMatrixCols);
		}else{
			fprintf(stderr, "First matrix cols must be the same length as the second matrix rowx.\n");
			exit(EXIT_FAILURE);
		}

		if(num_threads > matrix1Rows){
			num_threads = matrix1Rows;
		}

		thread = (pthread_t*) malloc(sizeof(pthread_t) * num_threads);
		/* descriptors */
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

		for(i=0; i < num_threads; i++){
			if(pthread_create(&thread[i], NULL, (void *(*) (void *)) multiply, (void *)i) != 0){
				printf("Error creating threads \n");
			}
		}

		for(i=0; i < num_threads; i++){
      		pthread_join(thread[i], NULL);
		}

		if(writeMatrix(argv[4], multMatrix, multMatrixRows, multMatrixCols) != 1){
			fprintf(stderr, "Error when creating output file.\n");
			exit(EXIT_FAILURE);
		}

		// verificacao
	//	printMatrix(matrix1->matrix, matrix1Rows, matrix1->cols);
	//	printMatrix(matrix2->matrix, matrix2->rows, matrix2->cols);
	//  printMatrix(multMatrix->matrix, matrix1->cols, matrix2->rows);


		free(matrix1);
		free(matrix2);
		free(multMatrix);

    }

    return 0;
}
