#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define RCVBUFSIZE 32

void DieWithError(char *errorMessage);
void FileUploadProcess(int sock, char *A_fileName);
void get(int sock, char *A_fileName);

int main(int argc, char *argv[]){

    int sock;
    struct sockaddr_in echoServAddr;
    unsigned short echoServPort;
    char *servIP=malloc(sizeof(char)*10);
    char *echoString=malloc(sizeof(char)*RCVBUFSIZE);
    char echoBuffer[RCVBUFSIZE];
    char filename[RCVBUFSIZE];
    unsigned int echoStringLen;
    int bytesRcvd, totalBytesRcvd;
    int flag=0;
    int c;
    
    printf("server ip: ");
    scanf("%s",servIP);
    printf("port: ");
    scanf("%u", &echoServPort);
    while((c=getchar())!='\n'&&c!=EOF);

    if((sock=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))<0)
        DieWithError("socket() failed");
    
    //Construct the server address structure
    memset(&echoServAddr,0,sizeof(echoServAddr));
    echoServAddr.sin_family=  AF_INET;
    echoServAddr.sin_addr.s_addr=inet_addr(servIP);
    echoServAddr.sin_port=htons(echoServPort);
    
    //establish the connection to the echo server
    if(connect(sock,(struct sockaddr *)& echoServAddr, sizeof(echoServAddr))<0)
        DieWithError("connect() failed");
  
    while(strcmp("./quit",echoBuffer)){
    
        printf("=========== Command ==========\n");
        printf("1. send file\n");
        printf("2. download file\n");
        printf("3. save log\n");
        printf("4. chat\n");
        printf("==============================\n");
        
        memset(echoBuffer, 0, RCVBUFSIZE);
        
        printf("MyShell:~$");
        scanf("%s", echoBuffer);
        echoStringLen= strlen(echoBuffer);
        if(send(sock, echoBuffer,strlen(echoBuffer) ,0)!=echoStringLen)
            DieWithError("send() failed");

        
        if(strcmp("send file", echoBuffer)==0){
            memset(filename, 0,RCVBUFSIZE);
            printf("send file is working..\n");
            printf("file name:");
            scanf("%s", filename);
            FileUploadProcess(sock,filename);

        }
        else if(strcmp("download file", echoBuffer)==0){
        
            printf("download file is working..\n");
            memset(filename, 0, RCVBUFSIZE);
            printf("file name: ");
            scanf("%s",filename);
            get(sock, filename);
        }
        else if(strcmp("save log",echoBuffer)==0){
            
            printf("save log..\n");
        
        }
        else if(strcmp("chat",echoBuffer)==0){
            
            printf("chatting is now working,,\n");
        while(strcmp("./out",echoBuffer)){

                 memset(echoBuffer,0, sizeof(echoBuffer));
                 printf("message-> ");
                 scanf("%s", echoBuffer);
                 echoStringLen=strlen(echoBuffer);
                 if((send(sock, echoBuffer, strlen(echoBuffer), 0))!=echoStringLen)
                     DieWithError("send() failed");
                 
                 memset(echoBuffer, 0,sizeof(echoBuffer));
                 totalBytesRcvd=0;
                 printf("message<- : ");
                
                 while(totalBytesRcvd < echoStringLen){
    
                     //Receive up to the buffer size from the sender
                     //minus 1 to leave space for a null terminator
               
                 if((bytesRcvd=recv(sock,echoBuffer, RCVBUFSIZE-1, 0))<=0)
                        DieWithError("recv() failed or connection closed prematurely");
                    totalBytesRcvd += bytesRcvd;
                    echoBuffer[bytesRcvd]='\0';
                    printf(echoBuffer);
                  }   

                 printf("\n");

                /* if(recv(sock, echoBuffer, sizeof(echoBuffer),0)<=0)
                     DieWithError("recv() failed");

                 printf("message<- ");
                 printf(echoBuffer);
                 printf("\n");*/
            }
        }
        else{
        
            printf("!!! Invalid command !!!\n");
        }

    }
/*    while(1){
    
        echoStringLen=strlen(echoString);
        //send the string to the server
        if(send(sock, echoString, echoStringLen, 0)!=echoStringLen)
            DieWithError("send() failed");

         //if input message is "./quit" then break the while loop
        if(strcmp(echoString, "./quit")==0)break;
    
        // memset(echoString, 0, sizeof(echoString));
    
        //receive the same string back from the server
        totalBytesRcvd=0;
        printf("message<- : ");
        while(totalBytesRcvd < echoStringLen){
    
               //Receive up to the buffer size from the sender
               //minus 1 to leave space for a null terminator
               //
                if((bytesRcvd=recv(sock,echoBuffer, RCVBUFSIZE-1, 0))<=0)
                    DieWithError("recv() failed or connection closed prematurely");
                totalBytesRcvd += bytesRcvd;
                echoBuffer[bytesRcvd]='\0';
                printf(echoBuffer);
            }   

        printf("\n");

         if(strcmp(echoString, "FT")==0)handleOperation(sock);
        memset(echoString, 0, sizeof(echoString));
     
        printf("message->:");
        scanf("%s",echoString);
   
    }*/
    close(sock);
    exit(0);
}
