#include <math.h>
#include <pthread.h>

void *ComputePartition(void* rank, void* num_proc, void* dim, void* A, void* B, void* C, void* mutex){
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
    long t_rank = (long) *rank;
    int sqrt_proc = sqrt((double) *num_proc);
    int m_dim = (int) *dim;
    int*** m_A = (int***) A;
    int*** m_B = (int***) B;
    int*** m_C = (int***) C;
    pthread_mutext_t* c_mutex = mutex;

    // use x,y to determine bounds of partition based on rank of thread
    double x = floor(t_rank/sqrt_proc);
    double y = t_rank % sqrt_proc;

    // compute each element of partition and update results when safe
    for (int i = x*m_dim/sqrt_proc; i < (x+1)*m_dim/sqrt_proc; i++){
        int el_ij = 0;
        for (int j = y*m_dim/sqrt_proc; j < (y+1)*m_dim/sqrt_proc; j++){
            for (int d = 0; d < m_dim; d++){
                el_ij += (*m_A[i][d]) * (*m_A[d][j]);
            }
            pthread_mutex_lock(&c_mutex);
            *m_C[i][j] = el_ij;
            pthread_mutex_unlock(&c_mutex);
        }
    }
    
    return NULL;
}

