#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include "matrix.h"

int matrix1_rows, matrix1_cols, *matrix1;
int matrix2_rows, matrix2_cols, *matrix2;
int matrix_out_rows, matrix_out_cols;

int process_count = 0;
int process_rank = 0;
pid_t pid = -1;

void multiplyMatrices();

int main (int argc, char **argv) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <processcount>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    process_count = strtol(argv[3], NULL, 10);
    if (process_count == 0) {
        fprintf(stderr, "Error: process count cannot be zero.\n");
        exit(EXIT_FAILURE);
    }

    matrix1 = readMatrix(argv[1], &matrix1_rows, &matrix1_cols);
    matrix2 = readMatrix(argv[2], &matrix2_rows, &matrix2_cols);

    if (matrix1_cols != matrix2_rows) {
        fprintf(stderr, "Error: these two matrices cannot be multiplied!\n");
        exit(EXIT_FAILURE);
    }

    matrix_out_rows = matrix1_rows;
    matrix_out_cols = matrix2_cols;

    // Create shared memory segment
    long int size = (matrix_out_rows * matrix_out_cols) * 4;
    int shmatrix_id = shmget(IPC_PRIVATE, size, S_IRUSR | S_IWUSR);

    pid_t *children_pid = malloc(sizeof(pid_t) * process_count);
    int i;
    // Create child processes
    for (i=0; i<process_count-1; i++) {
        // Parent process sets the rank for the child and forks
        if (pid != 0) {
            process_rank++;
            pid = fork();
            // Populates an array of ints with the pid of all children
            if (pid != 0) children_pid[i] = pid;
            else if (pid == -1) {
                fprintf(stderr, "Error while creating child processes.\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    if (pid != 0) {
        // Parent is always rank 0
        process_rank = 0;
    }

    // Attach pointer to shared memory segment
    int* shared_matrix = (int*) shmat(shmatrix_id, NULL, 0);
    multiplyMatrices(shared_matrix);

    if (process_rank == 0) {
        // Wait for all children to finish
        int return_status;
        for (i=0; i<process_count-1; i++) {
            waitpid(children_pid[i], &return_status, 0);
            if (return_status != 0) {
                fprintf(stderr, "Error: process with PID %d exited with status %d.\n", children_pid[i], return_status);
                exit(EXIT_FAILURE);
            }
        }

        writeMatrix(argv[4], shared_matrix, matrix_out_rows, matrix_out_cols);

        // Detach and free shared memory
        shmdt(shared_matrix);
        shmctl(shmatrix_id, IPC_RMID, NULL);

        free(matrix1);
        free(matrix2);
        free(children_pid);
    }
    else {
        // Detach shared memory
        shmdt(shared_matrix);
    }
    return 0;
}

void multiplyMatrices(int* shmem_matrix) {
    int i, j, k, sum=0;

    for(i=process_rank; i<matrix1_rows; i+=process_count) {
        for(j=0; j<matrix2_cols; j++) {
            for(k=0; k<matrix2_rows; k++) {
                sum += matrix1[(i*matrix1_rows) + k] * matrix2[(k*matrix2_rows) + j];
            }

            shmem_matrix[(i*matrix_out_rows)+j] = sum;
            sum = 0;
        }
    }
}
