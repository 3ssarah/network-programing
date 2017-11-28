#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define FILENAMESIZE 256
#define BUFSIZE 512

/*메세지 타입*/
#define EchoReq 01
#define FileUpReq 02
#define EchoRep 11
#define FileAck 12


void DieWithError(char* msg);
void HandleFTPUpload(int clnt_sock){//업로드 된 파일을 다루는 함수

    /*파일 이름 수신*/
    printf("--파일 이름 수신한다리--\n");
   char fileName[FILENAMESIZE];
   memset(fileName, 0,FILENAMESIZE);
   
   int numBytesRcvd;
   
   if(numBytesRcvd=recv(clnt_sock, fileName, FILENAMESIZE,0)==-1)
       DieWithError("recv() failed");
   
   else if(numBytesRcvd==0)DieWithError("peer connection closed");
   
   else if(numBytesRcvd!=FILENAMESIZE)DieWithError("recv unexpected number of bytes");
   
   strcat(fileName,"_up");
   
   printf("fileName = %s\n", fileName);
   
   /* 파일 크기 수신*/
   printf("--파일 크기를 수신한다리--\n");
   
   int netFileSize;
   int fileSize;
   
   if(numBytesRcvd=recv(clnt_sock,&netFileSize, sizeof(netFileSize),MSG_WAITALL)==-1)
       DieWithError("recv() failed");
   
   else if(numBytesRcvd==0)DieWithError("peer connection closed");
   
   else if(numBytesRcvd!= sizeof(netFileSize))DieWithError("recv unexpected number of bytes");
   
   fileSize= ntohl(netFileSize);
   
   printf("fileSize= %u\n",fileSize);

   /*파일 내용 수신*/
   printf("--파일 내용을 수신한다리--\n");
   FILE *fp= fopen(fileName,"w");
   if(fp==NULL)DieWithError("fopen() failed");

   int recvdFileSize=0;
   
   while(recvdFileSize<fileSize){
   
       char fileBuf[BUFSIZE];
       if(numBytesRcvd=recv(clnt_sock, fileBuf, BUFSIZE,0)==-1)
           DieWithError("recv() failed");
       else if(numBytesRcvd==0)DieWithError("peer connection closed");
        printf("%s\n",fileBuf);
       fwrite(fileBuf,sizeof(char), numBytesRcvd,fp);
       if(ferror(fp))DieWithError("fwrite() error");

       recvdFileSize += numBytesRcvd;
   }
   fclose(fp);//파일 수신 완료
   printf("--파일 수신 완료했다리--\n");

   /*파i일수신 성공 메시지(FileAck)를 클라이언트에게 전송*/
   printf("--ACK 메시지 날릴거다리--\n");
   char msgType=FileAck;
   int numBytesSent;
     if( numBytesSent =send(clnt_sock, &msgType, sizeof(msgType),0)==-1)
         DieWithError("send() failed");
     else if(numBytesSent!= sizeof(msgType))DieWithError("sent unexpected number of bytes");

}
void FileUploadProcess(int clnt_sock){//파일을 업로드하는 함수

    /*recv File name*/
    char fileName[FILENAMESIZE];
    if(recv(clnt_sock, fileName, FILENAMESIZE, 0)<0)
        DieWithError("recv() failed");
    
    /*send file size to client*/
    struct stat sb;
    if(stat(fileName, &sb)<0)DieWithError("stat() error");
    int fileSize= sb.st_size;
    int netFileSize=htonl(fileSize);
    
    if((send(clnt_sock, &netFileSize, sizeof(netFileSize),0))!=sizeof(netFileSize))
        DieWithError("send() failed");
    /*send file contents to client*/
    FILE *fp= fopen(fileName,"r");
    if(fp==NULL)DieWithError("fopen() failed");

    while(!feof(fp)){
        
        char fileBuf[BUFSIZE];
        int numBytesRead= fread(fileBuf, sizeof(char), BUFSIZE, fp);
        
        if(ferror(fp))DieWithError("ferror() failed");

        if(send(clnt_sock, fileBuf, numBytesRead, 0)!=numBytesRead)
            DieWithError("send() failed");

    }
    fclose(fp);
    
    char msgType;
    if(recv(clnt_sock, &msgType, sizeof(msgType),0)<0)
        DieWithError("recv() faile");
    if(msgType==FileAck)
        printf("%s 파일 전송 성공!!(%u Bytes)\n",fileName,fileSize);
    else printf("파일 전송 실패,,\n");
}


