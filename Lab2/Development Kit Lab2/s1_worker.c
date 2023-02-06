#include "common.h"

pthread_mutex_t arrMutex;

void InitArrayPro(int n){
    times_ind = 0;
    pthread_mutex_init(&arrMutex, NULL);
}

void *WorkerSingleMutex(void *args){
    double startTime, endTime;
    int clientFileDescriptor=(int)args;
    char cs_msg[COM_BUFF_SIZE];
    ClientRequest cr;

    read(clientFileDescriptor,cs_msg,COM_BUFF_SIZE);
    GET_TIME(startTime);

    ParseMsg(cs_msg, &cr);
    if (cr.is_read){
        pthread_mutex_lock(&arrMutex);
        getContent(cs_msg,cr.pos,theArray);
        pthread_mutex_unlock(&arrMutex);
    }
    else {
        pthread_mutex_lock(&arrMutex);
        setContent(cr.msg,cr.pos,theArray);
        getContent(cs_msg,cr.pos,theArray);
        pthread_mutex_unlock(&arrMutex);
    }

    GET_TIME(endTime); 
    write(clientFileDescriptor,cs_msg,COM_BUFF_SIZE);
    
    close(clientFileDescriptor);
    timeMng(endTime-startTime);
    return NULL;
}

void* WorkerFunc = WorkerSingleMutex;
