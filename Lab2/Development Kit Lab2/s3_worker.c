#include "common.h"

pthread_rwlock_t arrLock;

void InitArrayPro(int n){
    times_ind = 0;
    pthread_rwlock_init(&arrLock, NULL);
}

void *WorkerSingleLock(void *args){
    double startTime, endTime;
    int clientFileDescriptor=(int)args;
    char cs_msg[COM_BUFF_SIZE];
    ClientRequest cr;

    read(clientFileDescriptor,cs_msg,COM_BUFF_SIZE);
    GET_TIME(startTime);

    ParseMsg(cs_msg, &cr);
    if (cr.is_read){
        pthread_rwlock_rdlock(&arrLock);
        getContent(cs_msg,cr.pos,theArray);
        pthread_rwlock_unlock(&arrLock);
    }
    else {
        pthread_rwlock_wrlock(&arrLock);
        setContent(cr.msg,cr.pos,theArray);
        getContent(cs_msg,cr.pos,theArray);
        pthread_rwlock_unlock(&arrLock);
    }
    GET_TIME(endTime); 
    write(clientFileDescriptor,cs_msg,COM_BUFF_SIZE);

    close(clientFileDescriptor);
    timeMng(endTime-startTime);
    return NULL;
}

void* WorkerFunc = WorkerSingleLock;
