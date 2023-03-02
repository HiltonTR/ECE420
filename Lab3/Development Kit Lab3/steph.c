#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Lab3IO.h"
#include "timer.h"

#define TOL 0.0005

// shitty implementation of gaussian elim that paralizes inner loop instead

int main(int argc, char* argv[]){
	int i, j, k, size;
	double** Au;
	double* X;
	double temp;
	int* index;
    int thread_count;
    int matrix_size;

	// check threads entry from cmd line
	if (argc != 3){
		fprintf(stderr, "%s <number of threads> <matrix_size>\n", argv[0]);
		exit(0);
	}
	thread_count = strtol(argv[1], NULL, 10);
    matrix_size = strtol(argv[2], NULL, 10);
	if (thread_count <= 0){
			fprintf(stderr, "%s <number of threads> <matrix_size>\n", argv[0]);
		exit(0);
	}

	/*Load the datasize and verify it*/
	Lab3LoadInput(&Au, &size);
	/*Calculate the solution by serial code*/
	X = CreateVec(size);
	index = malloc(size * sizeof(int));

    double start, end;
	GET_TIME(start);
	for (i = 0; i < size; ++i)
		index[i] = i;

	if (size == 1)
		X[0] = Au[0][1] / Au[0][0];
	else{
		/*Gaussian elimination*/
		for (k = 0; k < size - 1; ++k){
			/*Pivoting*/
			temp = 0;
			for (i = k, j = 0; i < size; ++i)
				if (temp < Au[index[i]][k]){
					temp = Au[index[i]][k];
					j = i;
				}
			if (j != k)/*swap*/{
				i = index[j];
				index[j] = index[k];
				index[k] = i;
			}
			/*calculating*/
			for (i = k + 1; i < size; ++i){
				temp = Au[index[i]][k] / Au[index[k]][k];
                #pragma omp parallel for private(j) num_threads(thread_count)
				for (j = k; j < size + 1; ++j)
					Au[index[i]][j] -= Au[index[k]][j] * temp;
			}       
		}
		/*Jordan elimination*/
		for (k = size - 1; k > 0; --k){
            #pragma omp parallel for private(temp,i) num_threads(thread_count)
			for (i = k - 1; i >= 0; --i ){
				temp = Au[index[i]][k] / Au[index[k]][k];
				Au[index[i]][k] -= temp * Au[index[k]][k];
				Au[index[i]][size] -= temp * Au[index[k]][size];
			} 
		}
		/*solution*/
        #pragma omp parallel for private(k) num_threads(thread_count)
		for (k=0; k< size; ++k)
			X[k] = Au[index[k]][size] / Au[index[k]][k];
	}

	// get end time and print
	GET_TIME(end);

    FILE* op;
    char filename[30];
    sprintf(filename,"inner_loop_nt_%i_ms_%i",thread_count,matrix_size);
    if ((op = fopen(filename,"a+")) == NULL){
        printf("Error opening the output file: inner_loop.\n");
        exit(1);
    }
    fprintf(op,"%e\n", end-start);
    fclose(op);

	// save output
	Lab3SaveOutput(X, size, end-start);

	// free memory
	DestroyVec(X);
	DestroyMat(Au, size);
	free(index);
return 0;
}