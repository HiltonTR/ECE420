#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include "Lab3IO.h"
#include "timer.h"


int thread_count;

int main(int argc, char* argv[]) {
	int i, j, k, size;
	double** Augmented;
	double temp;

	// check threads entry from cmd line
	if (argc != 2){
		fprintf(stderr, "%s <number of threads>\n", argv[0]);
		exit(0);
	}
	thread_count = strtol(argv[1], NULL, 10);
	if (thread_count <= 0){
			fprintf(stderr, "%s <number of threads>\n", argv[0]);
		exit(0);
	}

	Lab3LoadInput(&Augmented, &size);

	double* Sol;
	Sol = CreateVec(size);

	int* index;
	index = malloc(size * sizeof(int));
	
	for (i = 0; i < size; ++i) {
		index[i] = i;
	}

	double start, end;
	GET_TIME(start);

	if (size == 1) {
		Sol[0] = Augmented[0][1] / Augmented[0][0];
	}
	else{
		// Gaussian elim
		for (k = 0; k < size - 1; ++k) {
            // Pivot
            double pivot = 0;
            j = 0;
            for (i = k; i < size; ++i) {
                if (pivot < Augmented[index[i]][k] * Augmented[index[i]][k]){
                    pivot = Augmented[index[i]][k] * Augmented[index[i]][k];
                    j = i;
                }
            }

			#pragma omp parallel for private(temp,i,j) num_threads(thread_count)
			for (i = k + 1; i < size; ++i) {
				temp = Augmented[index[i]][k] / Augmented[index[k]][k];
				for (j = k; j < size + 1; ++j)
					Augmented[index[i]][j] -= Augmented[index[k]][j] * temp;
			}	   
		}
		// Jordan elim
		for (k = size - 1; k > 0; --k) {
			#pragma omp parallel for private(temp,i) num_threads(thread_count)
			for (i = 0; i < k; i++ ) {
				temp = Augmented[index[i]][k] / Augmented[index[k]][k];
				Augmented[index[i]][k] -= temp * Augmented[index[k]][k];
				Augmented[index[i]][size] -= temp * Augmented[index[k]][size];
			}
		}
		// merge
		#pragma omp parallel for private(k) num_threads(thread_count)
		for (k=0; k< size; ++k) {
			Sol[k] = Augmented[index[k]][size] / Augmented[index[k]][k];
		}
	}
	// get end time and print
	GET_TIME(end);
	printf("%lf\n", end-start);

	// save output
	Lab3SaveOutput(Sol, size, end-start);

	// free memory
	DestroyVec(Sol);
	DestroyMat(Augmented, size);
	free(index);
	return 0;
}


