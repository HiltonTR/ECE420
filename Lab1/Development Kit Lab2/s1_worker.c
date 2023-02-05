#include "common.h"

pthread_mutex_t arrMutex;

void *WorkerSingleMutex(void *args)
{
    int clientFileDescriptor=(int)args;
    char cs_msg[COM_BUFF_SIZE];
    read(clientFileDescriptor,cs_msg,COM_BUFF_SIZE);

    ClientRequest cr;
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
    write(clientFileDescriptor,cs_msg,COM_BUFF_SIZE);
    close(clientFileDescriptor);
    return NULL;
}

void* WorkerFunc = WorkerSingleMutex;