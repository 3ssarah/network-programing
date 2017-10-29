#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define RCVBUFSIZE 32
#define MAXEXPENDING 5

void DieWithError(char *msg);

void HandleTCPClient(int clntSocket){

    char echoBuffer[RCVBUFSIZE];
    int recvMsgSize;
    int flag=0;
    
    //receive message from client
    if((recvMsgSize=recv(clntSocket, echoBuffer, RCVBUFSIZE,0))<0)
        DieWithError("recv() failed");
    printf("message <-:%s \n", echoBuffer);

    while(recvMsgSize>0){
    
        if(send(clntSocket, echoBuffer, recvMsgSize,0)!=recvMsgSize)
            DieWithError("send() failed");
        printf("message ->: %s\n", echoBuffer);

        memset(echoBuffer, 0, sizeof(echoBuffer));

        if((recvMsgSize=recv(clntSocket, echoBuffer, RCVBUFSIZE,0))<0)
            DieWithError("recv() failed");
        printf("message <- %s \n", echoBuffer);

        if(strcmp(echoBuffer, "./quit")==0)break;
    }
    close(clntSocket);
}

int AcceptTCPConnection(int servSock){

    int clntSock;
    struct sockaddr_in echoClntAddr;
    unsigned int clntLen;

    //set the size of the in-out parameter
    clntLen=sizeof(echoClntAddr);

    //wait for a client to connect
    if((clntSock=accept(servSock,(struct sockaddr *)&echoClntAddr, &clntLen))<0)
        DieWithError("accept() failed");
    //clntSock is connected to a client
    printf("Handleing client %s\n", inet_ntoa(echoClntAddr.sin_addr));
   
    return clntSock;
}
int CreateTCPServerSocket(unsigned short port){

    int sock;
    struct sockaddr_in echoServAddr;

    //create socket
    if((sock=socket(PF_INET, SOCK_STREAM,IPPROTO_TCP))<0)
        DieWithError("socket() failed");
    
    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family=AF_INET;
    echoServAddr.sin_addr.s_addr=htonl(INADDR_ANY);
    echoServAddr.sin_port=htons(port);

    if(bind(sock, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr))<0)
        DieWithError("bind() failed");

    if(listen(sock, MAXEXPENDING)<0)
        DieWithError("listen() failed");

    return sock;
}
