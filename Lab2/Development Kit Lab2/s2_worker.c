#include "common.h"

pthread_mutex_t *arrMutexes;

void InitArrayPro(int n){
    times_ind = 0;
    arrMutexes = malloc(n * sizeof(pthread_mutex_t)); // this memory is not explicitly freed, but this function is only called once and it will be reclaimed by OS upon program termination
    for (int i = 0; i < n ;i++){
        pthread_mutex_init(&arrMutexes[i], NULL);
    }
}

void *WorkerMultiMutex(void *args){   
    double startTime, endTime;
    int clientFileDescriptor=(int)args;
    char cs_msg[COM_BUFF_SIZE];
    ClientRequest cr;

    read(clientFileDescriptor,cs_msg,COM_BUFF_SIZE);
    GET_TIME(startTime);

    ParseMsg(cs_msg, &cr);
    if (cr.is_read){
        pthread_mutex_lock(&arrMutexes[cr.pos]);
        getContent(cs_msg,cr.pos,theArray);
        pthread_mutex_unlock(&arrMutexes[cr.pos]);
    }
    else {
        pthread_mutex_lock(&arrMutexes[cr.pos]);
        setContent(cr.msg,cr.pos,theArray);
        getContent(cs_msg,cr.pos,theArray);
        pthread_mutex_unlock(&arrMutexes[cr.pos]);
    }
    
    GET_TIME(endTime); 
    write(clientFileDescriptor,cs_msg,COM_BUFF_SIZE);
    
    close(clientFileDescriptor);
    timeMng(endTime-startTime);
    return NULL;
}

void* WorkerFunc = WorkerMultiMutex;
