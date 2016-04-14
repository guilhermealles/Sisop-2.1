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

	for(k = line; k < matrix1->rows; k += num_threads){
		for(i=0; i < matrix2->cols; i++){
			for(j=0; j < matrix1->rows; j++){
				result = result + (matrix1->matrix[k][j] * matrix2->matrix[j][i]);
			}
			multMatrix->matrix[k][i] = result;
			result = 0;
		}
	}

	return 0;
}

int main (int argc, char **argv) {
   if (argc == 4) {
		pthread_t * thread;
		int i=0;

        // read and allocate matrix
        matrix1 = readMatrix(argv[1], &matrix1Rows, &matrix1Cols);
		matrix2 = readMatrix(argv[2], &matrix2Rows, &matrix2Cols);

		num_threads = strtol(argv[3], NULL, 10);

		if(num_threads <= 0){
			fprintf(stderr, "Threads number must be higher than 0.\n")
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

		if(num_threads > matrix1->rows){
			num_threads = matrix1->rows;
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

		if(writeMatrix("out.txt", multMatrix) == 1){
			printf("Arquivo de saída gerado com sucesso.\n");
		}

		// verificacao
	//	printMatrix(matrix1->matrix, matrix1->rows, matrix1->cols);
	//	printMatrix(matrix2->matrix, matrix2->rows, matrix2->cols);
	//  printMatrix(multMatrix->matrix, matrix1->cols, matrix2->rows);


		free(matrix1);
		free(matrix2);
		free(multMatrix);

    }

    return 0;
}
