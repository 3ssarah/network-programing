#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#define RCVBUFSIZE 32
#define EchoReq 01
#define FileUpReq 02
#define EchoRep 11
#define FileAck 12
#define FileDownReq 13

void DieWithError(char *errorMessage);
void HandleFTPUpload(int clnt_sock);
void FileUploadProcess(int clnt_sock);
void send_ls(int clnt_sock);

void HandleClient(int clntSocket){

    char echoBuffer[RCVBUFSIZE];
    int recvMsgSize;
    int flag=0;
    char msg[3]="hi";
    char msgType;

    //Receive message from client
    if((recvMsgSize=recv(clntSocket,echoBuffer, RCVBUFSIZE,0))<0)
        DieWithError("recv() failed");
    printf("message <-: %s \n",echoBuffer);
    
    if(strcmp(echoBuffer,"hello")!=0){
        close(clntSocket);
    }
    else{
               while(recvMsgSize>0){/*zero indiactes end of transmission */
       
                   if(flag==0){
                       strcpy(echoBuffer, "hi");
                       flag++;
                   }
                       
                        if(send(clntSocket, echoBuffer, recvMsgSize,0)!= recvMsgSize)
                             DieWithError("send() failed");
                         printf("message ->: %s \n", echoBuffer);

                       
                       if(strcmp(echoBuffer,"FT")==0){
                            printf("Change to File Transfer-----\n ");
                          while(1){

                            if(recv(clntSocket, &msgType, sizeof(msgType),0)<0)
                                DieWithError("recv()   failed");
                            if(msgType==FileUpReq)
                              HandleFTPUpload(clntSocket);
                            if(msgType==FileDownReq)
                                FileUploadProcess(clntSocket);
                            if(msgType==EchoRep)
                                  send_ls(clntSocket);
                            if(msgType==EchoReq)break;
                           }
                       }
                       memset(echoBuffer,0,sizeof(echoBuffer));

                       if((recvMsgSize=recv(clntSocket,echoBuffer, RCVBUFSIZE,0))<0)
                           DieWithError("recv() failed");
                       printf("message <-: %s \n", echoBuffer);

                       //if(strcmp(echoBuffer,"FT")==0){
                       //       HandleFTPUpload(clntSocket);    
                       // }
                       if(strcmp(echoBuffer,"./quit")==0)break;
                    
               
               }
                           close(clntSocket);
    }
}

