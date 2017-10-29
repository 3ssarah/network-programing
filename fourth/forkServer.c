#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

#define MAXLINE 511
#define MAXEXPENDING 5

void DieWithError(char *errormsg);
void HandleTCPClient(int clnt_sock);
int AcceptTCPConnection(int serv_sock);

int main(int argc, char *argv[]){

    int serv_sock;
    int clnt_sock;
    unsigned short port;
    struct sockaddr_in servAddr;
    struct sockaddr_in clntAddr;
    int addlen, datalen;
    char buf[MAXLINE+1];
    int nbytes;
    pid_t processID;

    
    //실행시 port와 같이 입력
    if((argc<2)||(argc>2)){
        printf("Usage: %s <Port>\n",argv[0]);
        exit(1);
    }

    port=atoi(argv[1]);

    //create socket
    if((serv_sock=socket(PF_INET, SOCK_STREAM,IPPROTO_TCP))<0)
        DieWithError("socket() failed");

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family=AF_INET;
    servAddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servAddr.sin_port=htons(port);

    if(bind(serv_sock,(struct sockaddr *)&servAddr, sizeof(servAddr))<0)
        DieWithError("bind() failed");
   
    //create waiting Queue
    if(listen(serv_sock, MAXEXPENDING)<0)
        DieWithError("listen() failed");
    
    for(;;){
        //new connection creates a client sock
        clnt_sock=AcceptTCPConnection(serv_sock);
        //fork child process and report any errors
        processID=fork();
        if(processID<0)
            DieWithError("fork() failed");
        else if(processID==0){
       
            close(serv_sock);
            HandleTCPClient(clnt_sock);
            exit(0);
        }
    }

}
