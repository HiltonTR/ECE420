#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include "lab1_IO.h"
#include "sys/time.h"
#include "timer.h"
#include <stdlib.h>     // need this for EXIT_FAILURE and EXIT_SUCCESS

// Global variables
int thread_count;

int** matrix_A;
int** matrix_B;
int** matrix_C;
int size;

void *ComputePartition(void* rank, void* dim, void* A, void* B, void* C, void* mutex){
    /*
    Function for worker thread to compute a partition of matrix multiplication A x B = C

    -----
    Input:
    long *rank                pointer to rank of worker thread
    double *num_proc          pointer to total number of partitions/processes/threads
    int *dim                  pointer to the matrix size
    int ***A                  pointer to the matrix A
    int ***B                  pointer to the matrix B
    int ***C                  pointer to the matrix C, product matrix
    pthread_mutex_t *mutex    pointer to mutex used to protect matrix C

    -----
    Output:
    None, but modifies the content of matrix C
*/
    // cast void pointers to usable types
    long t_rank = (long) rank;
    int sqrt_proc = (int) sqrt(thread_count);
    // int m_dim = (int) dim;
    // int*** m_A = (int***) A;
    // int*** m_B = (int***) B;
    // int*** m_C = (int***) C;
    // pthread_mutex_t* c_mutex = mutex;


    // printf("m_dim %d \n", m_dim);
    // printf("size %d \n", size);

    // use x,y to determine bounds of partition based on rank of thread
    double x = floor(t_rank/sqrt(thread_count));
    double y = t_rank % sqrt_proc;

    // compute each element of partition and update results when safe
    for (int i = x*size/sqrt_proc; i < (x+1)*size/sqrt_proc; i++){
        
        for (int j = y*size/sqrt_proc; j < (y+1)*size/sqrt_proc; j++){
            int el_ij = 0;
            for (int d = 0; d < size; d++){
                el_ij += (matrix_A[i][d]) * (matrix_B[d][j]);
            }
            // pthread_mutex_lock(&c_mutex);
            matrix_C[i][j] = el_ij;
            // pthread_mutex_unlock(&c_mutex);

            printf("matrix_C[i][j] %d \n", matrix_C[i][j]);
        }
    }
    
    return NULL;
}

int main(int argc, char *argv[]) {

    pthread_t *threads;
    double start, end;

	// Checks args
	if (argc < 2) {
        printf("Invalid input. Correct usage: ./main <thread count> \n");
        return EXIT_FAILURE;
	}

	// Load inputs
	if (Lab1_loadinput(&matrix_A, &matrix_B, &size)) {
		return EXIT_FAILURE;
	}

    // Get thread count
    thread_count = atoi(argv[1]);

    // Allocate memory
    threads = malloc(thread_count*sizeof(pthread_t));

    matrix_C = malloc(size * sizeof(int*));
    for (int i = 0; i < size; i++) {
        matrix_C[i] = malloc(size * sizeof(int));
    }

	// Calculate start time
	GET_TIME(start);

    // create and join threads here
    for (int thread = 0; thread < thread_count; thread++) {
        pthread_create(&threads[thread], NULL, ComputePartition, (void*) thread);
    }

    for (int thread = 0; thread < thread_count; thread++){
        pthread_join(threads[thread], NULL);
    }


	// Calculate end time
	GET_TIME(end);

	// Print time taken
	printf("Time elapsed: %fs\n", (end - start));
	
	// Save output
	Lab1_saveoutput(matrix_C, &size, end - start);

    // Free memory
    for (int i = 0; i < size; i++) {
        free(matrix_A[i]); 
        free(matrix_B[i]); 
        free(matrix_C[i]);
    }
    free(matrix_A); 
    free(matrix_B); 
    free(matrix_C);
    free(threads);

	return EXIT_SUCCESS;
}
