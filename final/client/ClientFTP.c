#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>

#define FILENAMESIZE 256
#define BUFSIZE 256

/*메세지 타입*/
#define EchoReq 01
#define FileUpReq 02
#define EchoRep 11
#define FileAck 12
#define FileDownReq 13

void DieWithError(char *msg);
void FileUploadProcess(int sock, char *A_fileName);//파일 업로드
void get(int sock, char *A_fileName);//서버로부터 파일 받기

void FileUploadProcess(int sock, char *A_fileName){

    printf("--FileUploadProcess--\n");    
   
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
        memset(fileBuf, 0, sizeof(fileBuf));
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

void get(int sock, char* A_fileName){

    //msgType 전송
    char msgType=FileDownReq;
    int numBytesSent;
    if((numBytesSent=send(sock, &msgType, sizeof(msgType),0))!=sizeof(msgType))
        DieWithError("send() failed");
    
    //파일 이름 서버에 전송
    char fileName[FILENAMESIZE];
    memset(fileName, 0,FILENAMESIZE);
    strcpy(fileName, A_fileName);
    if((numBytesSent=send(sock, fileName, FILENAMESIZE,0))!=FILENAMESIZE)
        DieWithError("send() failed");
    
    //파일 크기를 수신
    printf("fileName= %s\n", fileName);
    int netFileSize;
    int fileSize;
    int numBytesRcvd=recv(sock,&netFileSize, sizeof(netFileSize),0);
    if(numBytesRcvd<0)DieWithError("recv() failed");
    fileSize= ntohl(netFileSize);

    //파일 내용을 수신
    FILE *fp= fopen(fileName,"w");
    if(fp==NULL)DieWithError("fopen() failed");
    memset(fileName, 0, sizeof(fileName));
    int rcvdFileSize=0;

        char fileBuf[BUFSIZE];
    
        if((numBytesRcvd=recv(sock,fileBuf, BUFSIZE,0))<0)
            DieWithError("recv() failed");
        fwrite(fileBuf, sizeof(char), numBytesRcvd, fp);
        
        if(ferror(fp))
            DieWithError("fwrite() failed");
        
        memset(fileBuf, 0, sizeof(fileBuf));
        printf("fileSize= %u Bytes  다운로드 완료\n", fileSize);
         fclose(fp);

    //파일수신 성공 메시지 전송
    msgType= FileAck;
    if((numBytesSent=send(sock, &msgType, sizeof(msgType),0))!=sizeof(msgType))
        DieWithError("send() failed");


}


void savelog(int sock, char* A_fileName){

    //msgType 전송
    char msgType=FileDownReq;
    int numBytesSent;
    if((numBytesSent=send(sock, &msgType, sizeof(msgType),0))!=sizeof(msgType))
        DieWithError("send() failed");
    
    //파일 이름 서버에 전송
    char string[FILENAMESIZE]="history.txt";
    printf("%s\n",string);
    if((numBytesSent=send(sock, string, FILENAMESIZE,0))!=FILENAMESIZE)
        DieWithError("send() failed");

    //파일 크기를 수신
    printf("fileName= %s\n", A_fileName);
    int netFileSize;
    int fileSize;
    int numBytesRcvd=recv(sock,&netFileSize, sizeof(netFileSize),0);
    if(numBytesRcvd<0)DieWithError("recv() failed");
    fileSize= ntohl(netFileSize);

    //파일 내용을 수신
    FILE *fp= fopen(A_fileName,"w");
    if(fp==NULL)DieWithError("fopen() failed");

    int rcvdFileSize=0;

    
        char fileBuf[BUFSIZE];
        if((numBytesRcvd=recv(sock,fileBuf, BUFSIZE,0))<0)
            DieWithError("recv() failed");
        fwrite(fileBuf, sizeof(char), numBytesRcvd, fp);
        if(ferror(fp))
            DieWithError("fwrite() failed");

        printf("fileSize= %u Bytes  다운로드 완료\n", fileSize);
         fclose(fp);

         memset(fileBuf, 0, sizeof(fileBuf));
    //파일수신 성공 메시지 전송
    msgType= FileAck;
    if((numBytesSent=send(sock, &msgType, sizeof(msgType),0))!=sizeof(msgType))
        DieWithError("send() failed");


}
