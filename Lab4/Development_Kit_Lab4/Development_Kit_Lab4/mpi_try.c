/*
    Test the result stored in "data_output" against a serial implementation.

    -----
    Compiling:
    Include "Lab4_IO.c" to compile. Set the macro "LAB4_EXTEND" defined in the "Lab4_IO.c" file to include the extended functions
    $ gcc serialtester.c Lab4_IO.c -o serialtester -lm 

    -----
    Return values:
    0      result is correct
    1      result is wrong
    2      problem size does not match
    253    no "data_output" file
    254    no "data_input_meta" or "data_input_link" file
*/
#define LAB4_EXTEND

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Lab4_IO.h"
#include "mpi.h"

#define EPSILON 0.00001
#define DAMPING_FACTOR 0.85

#define THRESHOLD 0.0001

int main (int argc, char* argv[]){
    struct node *nodehead;
    int nodecount;
    double *r, *r_pre, *contribution;
    int i, j;
    double damp_const;
    int iterationcount = 0;
    int collected_nodecount;
    double *collected_r;
    double cst_addapted_threshold;
    double error;
    FILE *fp, *ip;

    // Load the data and simple verification
    if ((fp = fopen("data_output", "r")) == NULL ){
    	printf("Error loading the data_output.\n");
        return 253;
    }
    fscanf(fp, "%d\n%lf\n", &collected_nodecount, &error);
    if ((ip = fopen("data_input_meta","r")) == NULL) {
        printf("Error opening the data_input_meta file.\n");
        return 254;
    }
    fscanf(ip, "%d\n", &nodecount);
    if (nodecount != collected_nodecount){
        printf("Problem size does not match!\n");
        return 2;
    }
    fclose(ip);
    collected_r = malloc(collected_nodecount * sizeof(double));
    for ( i = 0; i < collected_nodecount; ++i)
        fscanf(fp, "%lf\n", &collected_r[i]);
    fclose(fp);

    // Adjust the threshold according to the problem size
    cst_addapted_threshold = THRESHOLD;
    if (node_init(&nodehead, 0, nodecount)) return 254;


    // Start MPI and divide nodes between processes
    int my_rank;
    int my_lowest_node_inc;
    int my_highest_node_ex;
    int process_count;
    int pad_add_count;
    int padded_nodecount;
    int nodes_per_process;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &process_count);

    // single process allocates space, others must wait
    if (my_rank == 0){
        // in case num nodes not perfectly divisible by num processes
        pad_add_count = nodecount % process_count;
        padded_nodecount = nodecount + pad_add_count;
        nodes_per_process = padded_nodecount/process_count;
        r = malloc(padded_nodecount * sizeof(double));
        r_pre = malloc(padded_nodecount * sizeof(double));
        contribution = malloc(padded_nodecount * sizeof(double));
    }
    MPI_Barrier(MPI_COMM_WORLD);

    // each process computes what nodes its responsible for and initializes their values
    my_lowest_node_inc = my_rank * nodes_per_process;
    my_highest_node_ex = (my_rank+1) * nodes_per_process;
    
    for ( i = my_lowest_node_inc; i < my_highest_node_ex; ++i)
        r[i] = 1.0 / nodecount;
    for ( i = my_lowest_node_inc; i < my_highest_node_ex; ++i)
        contribution[i] = r[i] / nodehead[i].num_out_links * DAMPING_FACTOR;
    damp_const = (1.0 - DAMPING_FACTOR) / nodecount;
    
    // CORE CALCULATION FIX ME PARALELIZE AND END PARALIZATION
    // GET_TIME(start);
    do{
        ++iterationcount;
        vec_cp(r, r_pre, nodecount);
        // update the value
        for ( i = 0; i < nodecount; ++i){
            r[i] = 0;
            for ( j = 0; j < nodehead[i].num_in_links; ++j)
                r[i] += contribution[nodehead[i].inlinks[j]];
            r[i] += damp_const;
        }
        // update and broadcast the contribution
        for ( i=0; i<nodecount; ++i){
            contribution[i] = r[i] / nodehead[i].num_out_links * DAMPING_FACTOR;
        }
    }while(rel_error(r, r_pre, nodecount) >= EPSILON);
    // GET_TIME(end);
    // printf("Program converged at %d th iteration.\nElapsed time %f.\n", iterationcount, end-start);

    // post processing
    node_destroy(nodehead, nodecount);
    free(contribution);
    

}
