#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define MAXPENDING 5 /*최대 연결 가능 개수*/
#define RCVBUFSIZE 32

void DieWithError(char *errorMessage);
void HandleClient(int clntSocket);

int main(int argc, char *argv[]){

    int servSock;
    int clntSock;
    struct sockaddr_in echoServAddr;
    struct sockaddr_in echoClntAddr;
    unsigned short echoServPort=12345;
    unsigned int clntLen;
    
    //소켓 생성하기
    if((servSock=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))<0)
        DieWithError("socket() failed");

    //construct loacl address structure
    memset(&echoServAddr,0, sizeof(echoServAddr));
    echoServAddr.sin_family=AF_INET;
    echoServAddr.sin_addr.s_addr=htonl(INADDR_ANY);
    echoServAddr.sin_port=htons(echoServPort);

    //bind to the local address
    if(bind(servSock, (struct sockaddr*)&echoServAddr, sizeof(echoServAddr))<0)
        DieWithError("bind() failed");
    //mark the socket so it will listen for incoming connections
    if(listen(servSock, MAXPENDING)<0)
        DieWithError("listen() failed");


        //set the size of the in-out connect
        clntLen= sizeof(echoClntAddr);

        //wait for a client to connect
        if((clntSock=accept(servSock, (struct sockaddr *)&echoClntAddr,&clntLen))<0)
            DieWithError("accept() failed");

    

        printf("Handling client %s\n",inet_ntoa(echoClntAddr.sin_addr));
        printf("port number:%d\n ",echoClntAddr.sin_port);
        HandleClient(clntSock);


}
