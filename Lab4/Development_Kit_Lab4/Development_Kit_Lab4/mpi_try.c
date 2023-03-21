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


    
    int my_rank;
    int my_lowest_node_inc;
    int my_highest_node_ex;
    double *my_r;
    double *my_contribution;
    int process_count;
    int pad_add_count;
    int padded_nodecount;
    int nodes_per_process;

    // Start MPI and divide nodes between processes
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &process_count);

    pad_add_count = nodecount % process_count;
    padded_nodecount = nodecount + pad_add_count; // in case num nodes not perfectly divisible by num processes
    nodes_per_process = padded_nodecount / process_count;
    damp_const = (1.0 - DAMPING_FACTOR) / nodecount;

    // single process allocates space initializes data structures, others must wait
    if (my_rank == 0){
        r = malloc(padded_nodecount * sizeof(double));
        contribution = malloc(padded_nodecount * sizeof(double));

        for ( i = 0; i < nodecount; ++i)
            r[i] = 1.0 / nodecount;
        contribution = malloc(nodecount * sizeof(double));
        for ( i = 0; i < nodecount; ++i)
            contribution[i] = r[i] / nodehead[i].num_out_links * DAMPING_FACTOR;
        
        MPI_Bcast(r, padded_nodecount, double, 0, MPI_COMM_WORLD);
        MPI_Bcast(contribution, padded_nodecount, double, 0, MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    // each process computes what nodes its responsible for
    my_lowest_node_inc = my_rank * nodes_per_process;
    my_highest_node_ex = (my_rank+1) * nodes_per_process;
    my_r = malloc(nodes_per_process * sizeof(double));
    my_contribution = malloc(nodes_per_process * sizeof(double));
    my_r_pre = malloc(nodes_per_process * sizeof(double));
    

    // CORE CALCULATION
    if (my_rank == 0)
        GET_TIME(start);
    do{
        ++iterationcount;
        vec_cp(my_r, my_r_pre, nodes_per_process);
        // update the value
        int my_ind = 0;
        for (i = my_lowest_node_inc; i < my_highest_node_ex; ++i){
            my_r[my_ind] = 0;
            for (j = 0; j < nodehead[i].num_in_links; ++j)
                my_r[my_ind] += contribution[nodehead[i].inlinks[j]];
            my_r[my_ind] += damp_const;
            my_ind++;
        }
        // update and broadcast the contribution
        my_ind = 0;
        for (i = my_lowest_node_inc; i < my_highest_node_ex; ++i){
            my_contribution[my_ind] = r[i] / nodehead[i].num_out_links * DAMPING_FACTOR;
            my_ind++;
        }
        // update global r, contribution
        MPI_Allgather(my_r, nodes_per_process, double, r, nodes_per_process, double, MPI_COMM_WORLD);
        MPI_Allgather(my_contribution, nodes_per_process, double, contribution, nodes_per_process, double, MPI_COMM_WORLD);
        // wait for all processes to update globals before continuing
        MPI_Barrier(MPI_COMM_WORLD);
    
    }while(rel_error(my_r, my_r_pre, nodes_per_process) >= EPSILON);
    
    // wait for all processes to finish
    MPI_Barrier(MPI_COMM_WORLD);
    
    // thread 0 responsible for timing/printing
    if (my_rank == 0){
        GET_TIME(end);
        printf("Program converged at %d th iteration.\nElapsed time %f.\n", iterationcount, end-start); 
    }
    
    // post processing
    MPI_Finalize();
    node_destroy(nodehead, nodecount);
    free(contribution);
}
