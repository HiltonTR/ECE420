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
#include <mpi.h>

#include "Lab4_IO.h"
#include "timer.h"

#define EPSILON 0.00001
#define DAMPING_FACTOR 0.85
#define THRESHOLD 0.0001


int main(int argc, char* argv[]) {
    struct node *nodehead;
    int nodecount;
    double *r, *contribution;
    int i, j;
    double damp_const;
    int iterationcount = 0;


    int my_rank;
    int my_lowest_node_inc, my_highest_node_ex;

    double *my_r;
    double *my_contribution;
    double *my_r_pre;

    int process_count;
    int nodes_per_process, padded_nodecount;
    int cont;

    double start, end;

    FILE *ip;

    // load data
    if ((ip = fopen("data_input_meta","r")) == NULL) {
        printf("Error opening the data_input_meta file.\n");
        return 254;
    }
    fscanf(ip, "%d\n", &nodecount);
    if (node_init(&nodehead, 0, nodecount)){
        printf("Error in call to node_init\n");
        return 1;
    }
    printf("nodecount is initially: %i\n",nodecount);
    fclose(ip);

    // Start MPI and divide nodes between processes
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &process_count);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    nodes_per_process = (int) (nodecount/process_count);
    if (nodes_per_process * process_count != nodecount) {
        nodes_per_process += 1;
    }
    padded_nodecount = nodes_per_process * process_count;
    damp_const = (1.0 - DAMPING_FACTOR) / nodecount;

    // single process ainitializes data structures, others must wait
    r = malloc(padded_nodecount * sizeof(double));

    for ( i = 0; i < nodecount; ++i)
        r[i] = 1.0 / nodecount;
    contribution = malloc(padded_nodecount * sizeof(double));
    for ( i = 0; i < nodecount; ++i)
        contribution[i] = r[i] / nodehead[i].num_out_links * DAMPING_FACTOR;

    MPI_Bcast(r, nodecount, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(contribution, nodecount, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    // each process computes what nodes its responsible for
    my_lowest_node_inc = my_rank*nodes_per_process;
    my_highest_node_ex = (my_rank+1)*nodes_per_process - 1;
    my_r = malloc(nodes_per_process * sizeof(double));
    my_contribution = malloc(nodes_per_process * sizeof(double));
    my_r_pre = malloc(padded_nodecount * sizeof(double));

    // CORE CALCULATION
    if (my_rank == 0)
        GET_TIME(start);
    do{
        ++iterationcount;
        vec_cp(r, my_r_pre, nodecount);

        // update the value
        int my_ind = 0;
        for ( i = my_lowest_node_inc; i <= my_highest_node_ex && i < nodecount; ++i){
            my_ind = i-my_lowest_node_inc;
            my_r[my_ind] = 0;
            for ( j = 0; j < nodehead[i].num_in_links; ++j)
                my_r[my_ind] += contribution[nodehead[i].inlinks[j]];
            my_r[my_ind] += damp_const;
        }

        // update and broadcast the contribution
        for ( i=my_lowest_node_inc; i <= my_highest_node_ex && i<nodecount; ++i){
            my_ind = i-my_lowest_node_inc;
            my_contribution[my_ind] = my_r[my_ind] / nodehead[i].num_out_links * DAMPING_FACTOR;
        }

        MPI_Gather(my_r, nodes_per_process, MPI_DOUBLE, r, nodes_per_process, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Allgather(my_contribution, nodes_per_process, MPI_DOUBLE, contribution, nodes_per_process, MPI_DOUBLE, MPI_COMM_WORLD);
        // wait for all processes to update globals before continuing
        if (my_rank == 0) {
            cont = (rel_error(r, my_r_pre, nodecount) >= EPSILON);
        }
        MPI_Bcast(&cont, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }while(cont);

    // thread 0 responsible for timing/printing
    if (my_rank == 0){
        GET_TIME(end);
        double Time = end-start;
        printf("Elapsed time %f.\n", Time);
        printf("%i\n",nodecount); 
        Lab4_saveoutput(r, nodecount, Time);
    }
    
    // post processing
    MPI_Finalize();
    node_destroy(nodehead, nodecount);
    free(contribution);
    return 0;
}

