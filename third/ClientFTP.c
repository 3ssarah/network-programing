#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>

#define FILENAMESIZE 256
#define BUFSIZE 512
/*메세지 타입*/
#define EchoReq 01
#define FileUpReq 02
#define EchoRep 11
#define FileAck 12

void DieWithError(char *msg);
int handleOperation(int sock);
void FileUploadProcess(int sock, char *A_fileName);

int handleOperation(int sock){
printf("--handleOperation--\n");
    
    char operation;
    char c;
    int flag=0;
    char fileName[50];
    
    while((c=getchar())!='\n' && c!=EOF);
          
    while(flag!=1){
          
          printf("Welcome to Socket FT Client \n");
          printf("ftp commnad [p)ut, g)et, l)s, r)ls, e)xit] -> ");
          scanf("%c", &operation);
          
         if(operation=='p'){
               printf("filename to put to server ->");
               scanf("%s",fileName);
          
               while((c=getchar())!='\n' && c!=EOF);
               FileUploadProcess(sock,fileName);
           }else if(operation=='g'){
               printf("get file from the server->");
           }else if(operation=='l'){

           }else if(operation=='r'){

           }else if(operation=='e'){
               break;
           }else{
               printf("Wrong operation! Try Again.\n");
           }
         /*switch(command){
    
            case 'p':
                printf("filename to put to server ->");
                scanf("%s",fileName);
                //함수
                FileUploadProcess(sock, fileName);
                break;
             case 'g':
                printf("file namen to get the file from server ->\n");
                printf("not now\n");
                break;
            case 'l': 
                break;
            case 'r':
                break;
            case 'e': uint8_t msgType= ExitReq;
                      ssize_t numBytesSent= send(sock, &msgType, sizeof(msgType),0);
                      if(numBytesSent!=sizeof(msgType))DieWithError("send() failed");
                      flag=1;
                      break;
            default: break;
         }*/

    }printf("end of while loop\n");
    return 0;
}

void FileUploadProcess(int sock, char *A_fileName){

    printf("--FileUploadProcess--\n");    
    //msgType전송: 필드 크기 1바이트로 고정
  /*  char msgType =FileUpReq;
    ssize_t numBytesSent = send(sock, &msgType, sizeof(msgType), 0);
    if(numBytesSent!=sizeof(msgType))DieWithError("send() failed");
    printf("msgType전송 성공!\n");
*/
    //파일 이름을 서버에 전송:필드크기 256바이트로 고정
    int numBytesSent;
    char fileName[FILENAMESIZE];
    memset(fileName, 0, FILENAMESIZE);
    strcpy(fileName, A_fileName);
    if(numBytesSent=send(sock,fileName, FILENAMESIZE,0)!=FILENAMESIZE)
        DieWithError("send() failed");
    printf("파일 이름 서버에 전송 성공!\n");

    //파일 크기를 서버에 전송: 필드크기는 uint32_t로 고정
    struct stat sb;
    if(stat(fileName,&sb)<0)DieWithError("stat() failed");
    int fileSize=sb.st_size;
    int netFileSize=htonl(fileSize);
    if(numBytesSent=send(sock,&netFileSize, sizeof(netFileSize),0)!=sizeof(netFileSize))
        DieWithError("send() failed");
    printf("파일 크기 서버에 전송 성공\n");

    //파일 내용을 서버에 전송
    FILE *fp = fopen(fileName,"r");
    if(fp==NULL)DieWithError("fopen() failed");

    while(!feof(fp)){
    
        char fileBuf[BUFSIZE];
        int numBytesRead =fread(fileBuf, sizeof(char), BUFSIZE,fp);
        if(ferror(fp))DieWithError("fread() failed");

        if(numBytesSent=send(sock, fileBuf, numBytesRead, 0)!=numBytesRead)
            DieWithError("send() failed");

    }
    fclose(fp);//파일전송 완료
    printf("file contents send complite\n");

    //서버로 부터 ACK  수신 후 화면에 성공여부 출력
    char msgType;
    int numBytesRcvd=recv(sock,&msgType, sizeof(msgType), 0);
    if(numBytesRcvd<0)DieWithError("recv() failed");

    if(msgType==FileAck)printf("%s 파일업로드 성공!(%u Bytes)\n", fileName, fileSize);
    else printf("%s 파일업로드 실패!\n", fileName);
}

