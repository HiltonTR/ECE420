#include "common.h"

pthread_rwlock_t *arrLocks;

void InitArrayPro(int n){
    arrLocks = malloc(n * sizeof(pthread_rwlock_t)); // this memory is not explicitly freed, but this function is only called once and it will be reclaimed by OS upon program termination
    for (int i = 0; i < n ;i++){
        pthread_rwlock_init(&arrLocks[i], NULL);
    }
}

void *WorkerMultiLock(void *args)
{
    int clientFileDescriptor=(int)args;
    char cs_msg[COM_BUFF_SIZE];
    read(clientFileDescriptor,cs_msg,COM_BUFF_SIZE);

    ClientRequest cr;
    ParseMsg(cs_msg, &cr);
    if (cr.is_read){
        pthread_rwlock_rdlock(&arrLocks[cr.pos]);
        getContent(cs_msg,cr.pos,theArray);
        pthread_rwlock_unlock(&arrLocks[cr.pos]);
    }
    else {
        pthread_rwlock_wrlock(&arrLocks[cr.pos]);
        setContent(cr.msg,cr.pos,theArray);
        getContent(cs_msg,cr.pos,theArray);
        pthread_rwlock_unlock(&arrLocks[cr.pos]);
    }
    write(clientFileDescriptor,cs_msg,COM_BUFF_SIZE);
    close(clientFileDescriptor);
    return NULL;
}

void* WorkerFunc = WorkerMultiLock;
