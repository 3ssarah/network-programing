#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <dirent.h>

#define FILENAMESIZE 256
#define BUFSIZE 512

/*메세지 타입*/
#define EchoReq 01
#define FileUpReq 02
#define EchoRep 11
#define FileAck 12

void send_ls(int clnt_sock);
void DieWithError(char* msg);
void FileUploadProcess(int clnt_sock);
void HandleFTPUpload(int clnt_sock){

    /*파일 이름 수신*/
    printf("--파일 이름 수신한다리--\n");
   char fileName[FILENAMESIZE];
   memset(fileName, 0,FILENAMESIZE);
   int numBytesRcvd;
   if(numBytesRcvd=recv(clnt_sock, fileName, FILENAMESIZE,0)<0)
       DieWithError("recv() failed");
   strcat(fileName,"_up");
   printf("fileName = %s\n", fileName);
   
   /* 파일 크기 수신*/
   printf("--파일 크기를 수신한다리--\n");
   int netFileSize;
   int fileSize;
   if(numBytesRcvd=recv(clnt_sock,&netFileSize, sizeof(netFileSize),MSG_WAITALL)<0)
       DieWithError("recv() failed");
   fileSize= ntohl(netFileSize);
   printf("fileSize= %u\n",fileSize);

   /*파일 내용 수신*/
   printf("--파일 내용을 수신한다리--\n");
   FILE *fp= fopen(fileName,"w");
   if(fp==NULL)DieWithError("fopen() failed");

   int recvdFileSize=0;
   
   
       char fileBuf[BUFSIZE];
       if(numBytesRcvd=recv(clnt_sock, fileBuf, BUFSIZE,0)<0)
           DieWithError("recv() failed");
        printf("%s\n",fileBuf);
       fwrite(fileBuf,sizeof(char), numBytesRcvd,fp);
       if(ferror(fp))DieWithError("fwrite() error");
       printf("numBytesRcvd: %d\n",numBytesRcvd);
       recvdFileSize += numBytesRcvd;
   
   fclose(fp);//파일 수신 완료
   printf("--파일 수신 완료했다리--\n");

   /*파i일수신 성공 메시지(FileAck)를 클라이언트에게 전송*/
   printf("--ACK 메시지 날릴거다리--\n");
   char msgType=FileAck;
   int numBytesSent;
     if( numBytesSent =send(clnt_sock, &msgType, sizeof(msgType),0)!=sizeof(msgType))
         DieWithError("send() failed");
    // else if(numBytesSent!= sizeof(msgType))DieWithError("sent unexpected number of bytes");

}

void FileUploadProcess(int clnt_sock){

    //파일 이름을 수신
    char fileName[FILENAMESIZE];
    if(recv(clnt_sock, fileName, FILENAMESIZE, 0)<0)
        DieWithError("recv() failed");

    //파일 크기를 클라이언트에 전송
    struct stat sb;
    if(stat(fileName, &sb)<0)DieWithError("stat() error");
    int fileSize=sb.st_size;
    int netFileSize=htonl(fileSize);
    if(send(clnt_sock, &netFileSize, sizeof(netFileSize),0)!=sizeof(netFileSize))
        DieWithError("send failed()");

    //파일내용을 클라이언트에 전송
    printf("파일내용 전송 시작한다리---\n");
    FILE *fp=fopen(fileName, "r");
    if(fp==NULL)DieWithError("fopen(0 failed");

    while(!feof(fp)){
    
        char fileBuf[BUFSIZE];
        int numBytesRead= fread(fileBuf, sizeof(char), BUFSIZE, fp);
        if(ferror(fp))DieWithError("fread() failed");

        if(send(clnt_sock, fileBuf, numBytesRead,0)!=numBytesRead)
            DieWithError("send() failed");
    }
    fclose(fp);
    printf("파일 전송 완료----\n");

    char msgType;
    if(recv(clnt_sock, &msgType, sizeof(msgType),0)<0)
        DieWithError("recv() failed");
    if(msgType==FileAck)
        printf("%s 파일 전송 성공!!(%u Bytes)\n",fileName, fileSize);
    else printf("%s 파일 전송 실패,,\n", fileName);

}
void send_ls(int clnt_sock){

    struct dirent **d;
    int i,n=0;

    //현재 위치에서 파일 목록
    if(n=scandir(".",&d,NULL,NULL)<0){
    
        perror("scandir");
        exit(1);
    }
    //목록리스트 보내기
    if(send(clnt_sock, &n, sizeof(n),0)!=sizeof(n))
        DieWithError("send() sent a different number of bytes than expected");

    for(i=0;i<n; i++){
    
        //파일 이름 보내기
        if(send(clnt_sock, &(d[i]-> d_name),FILENAMESIZE,0)!=FILENAMESIZE)
           DieWithError("send() sent a different number of bytes than expected");
    }
}
