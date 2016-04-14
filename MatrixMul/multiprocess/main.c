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
pthread_mutex_t shm_mutex;

void multiplyMatrices();

int main (int argc, char **argv) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <processcount>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    process_count = strtol(argv[1], NULL, 10);
    if (process_count == 0) {
        fprintf(stderr, "Error: process count cannot be zero.\n");
        exit(EXIT_FAILURE);
    }

    matrix1 = readMatrix(argv[2], &matrix1_rows, &matrix1_cols);
    matrix2 = readMatrix(argv[3], &matrix2_rows, &matrix2_cols);

    if (matrix1_cols != matrix2_rows) {
        fprintf(stderr, "Error: these two matrices cannot be multiplied!\n");
        exit(EXIT_FAILURE);
    }

    matrix_out_rows = matrix1_rows;
    matrix_out_cols = matrix2_cols;

    // Create shared memory segment
    long int size = (matrix_out_rows * matrix_out_cols) * 4;
    int shmatrix_id = shmget(IPC_PRIVATE, size, S_IRUSR | S_IWUSR);
    int sh_done_count_id = shmget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR);

    // Initialize Mutex
    pthread_mutexattr_t attributes;
    pthread_mutexattr_init(&attributes);
    pthread_mutexattr_setpshared(&attributes, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&shm_mutex, &attributes);
    pthread_mutexattr_destroy(&attributes);

    int i;
    // Create child processes
    for (i=0; i<process_count-1; i++) {
        // Parent process sets the rank for the child and forks
        if (pid != 0) {
            process_rank++;
            pid = fork();
            if (pid == -1) {
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
    char* shared_done_count = (char*) shmat(sh_done_count_id, NULL, 0);

    multiplyMatrices(shared_matrix);
    //MUTEX
    pthread_mutex_lock(&shm_mutex);
    shared_done_count[0]++;
    pthread_mutex_unlock(&shm_mutex);
    //END MUTEX

    if (process_rank == 0) {
        while((int)shared_done_count[0] != process_count) {
            // Busy waiting until all processes are done
        }
        writeMatrix(argv[4], shared_matrix, matrix_out_rows, matrix_out_cols);

        // Detach and free shared memory
        shmdt(shared_matrix);
        shmdt(shared_done_count);
        shmctl(shmatrix_id, IPC_RMID, NULL);
        shmctl(sh_done_count_id, IPC_RMID, NULL);

        // Destroy mutex structure
        pthread_mutex_destroy(&shm_mutex);

        //TODO Free 2 input matrices
    }
    else {
        // Detach shared memory
        shmdt(shared_matrix);
        shmdt(shared_done_count);

        // Destroy mutex as well?
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
