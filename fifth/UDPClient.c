#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFSIZE 32
void DieWithError(char *msg);

int main(int argc, char *argv[]){

    char *servIP= malloc(sizeof(char)*10);
    char *echoServPort=malloc(sizeof(char)*10);

    char c;

    printf("server IP:");
    scanf("%s", servIP);
    printf("Port:");
    scanf("%s", echoServPort);
    while((c=getchar())!='\n' && c!=EOF);

    struct addrinfo addrCriteria;
    memset(&addrCriteria, 0, sizeof(addrCriteria));
    addrCriteria.ai_family=AF_UNSPEC;
    addrCriteria.ai_socktype=SOCK_DGRAM;
    addrCriteria.ai_protocol=IPPROTO_UDP;

    struct addrinfo *servAddr;
    int returnValue= getaddrinfo(servIP, echoServPort,&addrCriteria, &servAddr);
    if(returnValue!=0)
        DieWithError("getaddrinfo() failed");

    int sock=socket(servAddr->ai_family,servAddr->ai_socktype, servAddr->ai_protocol);
    if(sock<0)
        DieWithError("socket() failed");

    struct sockaddr_storage fromAddr;
    socklen_t fromAddrLen= sizeof(fromAddr);

    char buffer[BUFSIZE];
    memset(buffer, '\0', BUFSIZE);
    while(strcmp(buffer,"./quit")){
    
        printf("msg-> ");
        scanf("%s",buffer);

        ssize_t numByte= sendto(sock, buffer, strlen(buffer), 0, servAddr->ai_addr, servAddr->ai_addrlen);
        if(numByte <0)
            DieWithError("sendto() failed");
        else if(numByte!=strlen(buffer))
            DieWithError("sendto() error- sent unexpected number of bytes");

        numByte=recvfrom(sock, buffer, BUFSIZE, 0, (struct sockaddr *)&fromAddr, &fromAddrLen);
        if(numByte<0)
            DieWithError("recvfrom() failed");
        else if(numByte != strlen(buffer))
            DieWithError("recvfrom() received a packet from unknown source");

        buffer[numByte]='\0';
        printf("msg<- %s\n", buffer);

    }
     freeaddrinfo(servAddr);
     close(sock);
     return 0;
}
