#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#define RCVBUFSIZE 32
void DieWithError(char *errorMessage);

void HandleClient(int clntSocket){

    char echoBuffer[RCVBUFSIZE];
    int recvMsgSize;
    int flag=0;
    char msg[3]="hi";

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

                        memset(echoBuffer,0,sizeof(echoBuffer));

                       if((recvMsgSize=recv(clntSocket,echoBuffer, RCVBUFSIZE,0))<0)
                           DieWithError("recv() failed");
                       printf("message <-: %s \n", echoBuffer);

                       if(strcmp(echoBuffer,"./quit")==0)break;
                    
               
               }
                           close(clntSocket);
    }
}
