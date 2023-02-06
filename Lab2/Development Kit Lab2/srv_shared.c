#include "common.h"

#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>



pthread_mutex_t time_mutex;
void timeMng(double time_diff){
    pthread_mutex_lock(&time_mutex);
    times[times_ind] = time_diff;
    times_ind++;
    if (times_ind == COM_NUM_REQUEST){
        saveTimes(times, COM_NUM_REQUEST);
        times_ind = 0;        
    }
    pthread_mutex_unlock(&time_mutex);
}

int main(int argc, char* argv[])
{
    // parse input args
     if (argc != 4){ 
        fprintf(stderr, "usage: %s <Size of theArray_ on server> <server ip> <server port>\n", argv[0]);
        exit(0);
    }
    int n = atoi(argv[1]);
    const char* s_ip = argv[2];
    int s_port = atoi(argv[3]);


    // set up the data structure
    char* initializationStr;
    initializationStr = malloc(COM_BUFF_SIZE);
    theArray = malloc(n * sizeof(char *));
    for (int i = 0; i< n; i++){
        theArray[i] = malloc(COM_BUFF_SIZE);
        char *dst = theArray[i];
        int length = MIN(strlen(initializationStr)+1, COM_BUFF_SIZE);
        for (int j=0; j<length; j++){
            dst[j] = initializationStr[j];
        }
    }
    free(initializationStr);


    // set up access protection to data structure
    InitArrayPro(n);


    // set up sockets, listen and connect, spawing threads to handle new cnxs
    struct sockaddr_in sock_var;
    int serverFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
    int clientFileDescriptor;
    int i;
    pthread_t t[COM_NUM_REQUEST];
    sock_var.sin_addr.s_addr=inet_addr(s_ip);
    sock_var.sin_port=s_port;
    sock_var.sin_family=AF_INET;
    pthread_attr_t tattr;
    pthread_attr_setdetachstate(&tattr,PTHREAD_CREATE_DETACHED); // clean up thread mem w/o explict join
    if(bind(serverFileDescriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0)
    {
        printf("socket has been created\n");
        listen(serverFileDescriptor,2000); 
        while(1)        //loop infinity
        {
            for(i=0;i<COM_NUM_REQUEST;i++) // allow COM_NUM_REQUEST simultaneous cnxs
            {
                clientFileDescriptor=accept(serverFileDescriptor,NULL,NULL);
                //printf("Connected to client %d\n",clientFileDescriptor);
                int ptc_ret = pthread_create(&t[i],&tattr,WorkerFunc,(void *)(long)clientFileDescriptor);
                if(ptc_ret!=0){
                    printf("could not create thread, errno: %i\n",ptc_ret);
                    break;
                }
            }
        }
        close(serverFileDescriptor);
    }
    else{
        printf("socket creation failed\n");
    }


    // free up dynamically allocated memory
    for (int i = 0; i < n; i++){
        free(theArray[i]);
    }
    free(theArray);
    return 0;
}