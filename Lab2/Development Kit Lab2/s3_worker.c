#include "common.h"

pthread_rwlock_t arrLock;

void InitArrayPro(int n){
    pthread_rwlock_init(&arrLock, NULL);
}

void *WorkerSingleLock(void *args)
{
    int clientFileDescriptor=(int)args;
    char cs_msg[COM_BUFF_SIZE];
    read(clientFileDescriptor,cs_msg,COM_BUFF_SIZE);

    ClientRequest cr;
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
    write(clientFileDescriptor,cs_msg,COM_BUFF_SIZE);
    close(clientFileDescriptor);
    return NULL;
}

void* WorkerFunc = WorkerSingleLock;
