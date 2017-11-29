#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>

#define FILENAMESIZE 256
#define BUFSIZE 256

/*메세지 타입*/
#define EchoReq 01
#define FileUpReq 02
#define EchoRep 11
#define FileAck 12
#define FileDownReq 13

void log_msg(char* msg);
void DieWithError(char* msg);

void HandleFTPUpload(int clnt_sock){//업로드 된 파일을 다루는 함수

    /*파일 이름 수신*/
   char fileName[FILENAMESIZE];
   memset(fileName, 0,FILENAMESIZE);
   
   int numBytesRcvd;
   
   if((numBytesRcvd=recv(clnt_sock, fileName, FILENAMESIZE,0))<0)
       DieWithError("recv() failed");
    log_msg(fileName);
 
   printf("fileName = %s\n", fileName);
   
   /* 파일 크기 수신*/
   int netFileSize;
   int fileSize;
   
   if(numBytesRcvd=recv(clnt_sock,&netFileSize, sizeof(netFileSize),0)<0)
       DieWithError("recv() failed");
   
   fileSize= ntohl(netFileSize);
   
   printf("fileSize= %u bytes \n",fileSize);
   log_msg("fileSize");
   char temp[256];
   printf("sproint\n");
   sprintf(temp,"%d", fileSize);
   printf("hello\n");
   log_msg(temp);
   

   /*파일 내용 수신*/
   FILE *fp= fopen(fileName,"w");
   if(fp==NULL)DieWithError("fopen() failed");
   memset(fileName, 0, sizeof(fileName));
   int recvdFileSize=0;
   
   while(recvdFileSize<fileSize){
   
       char fileBuf[BUFSIZE];
       memset(fileBuf, 0, sizeof(fileBuf));
       if((numBytesRcvd=recv(clnt_sock, fileBuf, BUFSIZE,0))==-1)
           DieWithError("recv() failed");
       else if(numBytesRcvd==0)DieWithError("peer connection closed");
       log_msg(fileBuf);
       fwrite(fileBuf,sizeof(char), numBytesRcvd,fp);
       if(ferror(fp))DieWithError("fwrite() error");

       recvdFileSize += numBytesRcvd;
       
   }
   fclose(fp);//파일 수신 완료

   /*파i일수신 성공 메시지(FileAck)를 클라이언트에게 전송*/
   char msgType=FileAck;
   log_msg(&msgType);
   int numBytesSent;
     if( numBytesSent =send(clnt_sock, &msgType, sizeof(msgType),0)<0)
         DieWithError("send() failed");

}
void FileUploadProcess(int clnt_sock){//파일을 업로드하는 함수

    
    /*recv FileDownReq*/
    char msgType;
    char tmp[256];
    if(recv(clnt_sock, &msgType,sizeof(msgType),0)<0)
        DieWithError("recv() failed");
    log_msg(&msgType);
    if(msgType==FileDownReq){
       
        char fileName[FILENAMESIZE];
        memset(fileName, 0,FILENAMESIZE);
        //recv File name
        if(recv(clnt_sock, fileName, FILENAMESIZE,0)<0)
            DieWithError("recv() failed");
         log_msg(fileName);   
    
         /*send file size to client*/
         struct stat sb;
        if(stat(fileName, &sb)<0)DieWithError("stat() error");
        int fileSize= sb.st_size;
        int netFileSize=htonl(fileSize);
         
        log_msg("file size");
        sprintf(tmp, "%d", fileSize);
        log_msg(tmp);
        //log_msg(itoa(fileSize));

        if((send(clnt_sock, &netFileSize, sizeof(netFileSize),0))!=sizeof(netFileSize))
        DieWithError("send() failed");
   
        /*send file contents to client*/
        FILE *fp= fopen(fileName,"r");
        if(fp==NULL)DieWithError("fopen() failed");
        //memset(fileName, 0, sizeof(fileName));
        //char fileBuf[BUFSIZE];
        while(!feof(fp)){
            printf("fp : %p\n", fp);
        //while((fgets(fileBuf, BUFSIZE, fp))!= NULL){
            printf("File read ongoing\n"); 
            char fileBuf[BUFSIZE];
            //long lSize = ftell(fp);
            int numBytesRead= fread(fileBuf, sizeof(char), BUFSIZE, fp);
            printf("fileBuf : %s\n", fileBuf);
           // fgets(fileBuf, BUFSIZE, fp);
           // fgets();
            if(ferror(fp))DieWithError("ferror() failed");

            if(send(clnt_sock, fileBuf, BUFSIZE, 0)!=BUFSIZE)
                 DieWithError("send() failed");
           // log_msg(fileBuf);
            memset(fileBuf, 0, sizeof(fileBuf));
         }
        printf("End file read\n");
        fclose(fp);

    
        char msgType;
        if(recv(clnt_sock, &msgType, sizeof(msgType),0)<0)
             DieWithError("recv() faile");
        log_msg(&msgType);

        if(msgType==FileAck)
            printf("%s 파일 전송 성공!!(%u Bytes)\n",fileName,fileSize);
         else printf("파일 전송 실패,,\n");

    }else printf("Wrong msgType!!\n");

}

