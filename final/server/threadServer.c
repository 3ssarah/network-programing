#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define MAXPENDING 5

void DieWithError(char *msg);
void HandleTCPClient(int clntSocket);
int CreateTCPServerSocket(unsigned short port);
int AcceptTCPConnection(int servSock);
void *ThreadMain(void *arg);

struct ThreadArgs{

    int clntSock;
};

int main(int argc, char *argv[]){
    
    int servSock;
    int clntSock;
    unsigned short port;
    pthread_t threadID;
    struct ThreadArgs *threadArgs;

    if(argc!=2){
    
        fprintf(stderr,"Usage: %s <Server Port>\n", argv[0]);
        exit(1);
    }
    port= atoi(argv[1]);
    servSock=CreateTCPServerSocket(port);
    
    //run forever
    for(;;){
    
        clntSock= AcceptTCPConnection(servSock);

        //create seperate memory for client argument
        if((threadArgs=(struct ThreadArgs *)malloc(sizeof(struct ThreadArgs)))==NULL)
            DieWithError("malloc() failed");
        threadArgs-> clntSock= clntSock;

        //create thread
        if(pthread_create(&threadID, NULL, ThreadMain, (void *)threadArgs)!=0)
            DieWithError("pthread_create() failed");
        printf("with thread %ld\n", (long int)threadID);
    }
    //not reached
}

void *ThreadMain(void *threadArgs){


    int clntSock;
    //guarantees that thread resources are de-allocated upon return
    pthread_detach(pthread_self());

    //Extract socket file descriptor from argumnet
    clntSock=((struct ThreadArgs *)threadArgs)->clntSock;
    free(threadArgs);

    HandleTCPClient(clntSock);

    return (NULL);
}
