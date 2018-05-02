#define _GNU_SOURCE     /* To get defns of NI_MAXSERV and NI_MAXHOST */
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "client_help.h"

int create_data_socket(int PORT){
	int datafd,data_connfd;
	struct sockaddr_in serv_addr;
	datafd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    bind(datafd, (struct sockaddr*)&serv_addr,sizeof(serv_addr));

     if(listen(datafd, 10) == -1)
    {
        printf("Failed to listen\n");
        return -1;
    }
    // data_connfd = accept(datafd, (struct sockaddr*)NULL ,NULL);
    // printf("%s\n","Connection Succeed, Starting Transfer Process");
    return datafd;

}

void get_client_IP(char* IP){
	struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;
    char host[NI_MAXHOST];
    //char IP[NI_MAXHOST];
    int s;
    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            //writprintf("%s IP Address %s\n", ifa->ifa_name, addressBuffer); 
        	s = getnameinfo( ifa->ifa_addr, (ifa->ifa_addr->sa_family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6) , IP , NI_MAXHOST , NULL , 0 , NI_NUMERICHOST);
        	//printf("address :%s\n", host);
        } else if (ifa->ifa_addr->sa_family == AF_INET6) { // check it is IP6
            // is a valid IP6 Address
            tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            //printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer); 

        } 
    }

    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
   
}

int get_filesize(FILE *fp){
    int prev=ftell(fp);
    fseek(fp, 0L, SEEK_END);
    int sz=ftell(fp);
    fseek(fp,prev,SEEK_SET); //go back to where we were
    return sz;
}

void read_file(int commandfd, int datafd, char* filename){
	char file_buf[FILE_LENGTH];
    char reply[LINE];
    char flag;
    int n,m;
    if ( (n = read(commandfd, reply, FILE_LENGTH)) > 0){
        reply[n] = 0;
    }
    bzero(file_buf,sizeof(file_buf));
    printf("%s\n",reply);
    read(datafd,&flag,1);    //flag that show if the data exist in server

    if(strncmp(&flag,"0",1)==0){
    	FILE *recv_fp=fopen(filename,"w+");
        while((n=read(datafd, file_buf, FILE_TRANSFER_LENGTH))>0){
            printf("Saving Filename: %s\n",filename);
            fwrite(file_buf,1,strlen(file_buf),recv_fp);
            bzero(file_buf,sizeof(file_buf));
        }
       

        fclose(recv_fp);
    }
}

void send_file(int commandfd, int datafd, char* filename){
    FILE *send_fp;
    int filesize=0;
    int count=0,start=0,tail=0,bread=0;
    char file_buf[FILE_LENGTH];
    char reply[LINE],flag;
    send_fp = fopen(filename,"rb");
    memset(file_buf, '\0', sizeof(file_buf));
    
    if(send_fp==NULL){  //if file does not exist
        printf("450 %s: No such file or directory",filename);
        write(datafd,"1",1);
    }
    else{  //file exist, do the transfer
        printf("200 Command OK\n");
        write(datafd,"0",1);
        filesize=get_filesize(send_fp); //get file size for "fread"
        count = filesize/FILE_TRANSFER_LENGTH;
        tail = filesize%FILE_TRANSFER_LENGTH;

        //If file size if too big, we need to send with several fragment
        for(int i =0;i<count;i++){
            fseek(send_fp,start,SEEK_SET);
            bread = fread(file_buf,1,FILE_TRANSFER_LENGTH,send_fp);//read content from file
            write(datafd,file_buf,strlen(file_buf));//write to destination
            start=FILE_TRANSFER_LENGTH*(i+1);
            bzero(file_buf,sizeof(file_buf));
        }

        //send the rest
        fseek(send_fp,start,SEEK_SET);
        bread = fread(file_buf,1,tail,send_fp);
        write(datafd,file_buf,strlen(file_buf));
        bzero(file_buf,sizeof(file_buf));
    }
    fclose(send_fp);    

}

void read_ls(int commandfd, int datafd){
    char reply[LINE], result[LINE],flag;
    int n=0;
    if((n=read(commandfd,reply,sizeof(reply)))>0){
        reply[n]=0;
    }
    printf("%s\n",reply);
    printf("-----------------\n");
    read(datafd,&flag,1);

  
    
    if(strncmp(&flag,"0",1)==0){
        if((n=read(datafd,result,sizeof(result)))>0){
        result[n]=0;
        }
        printf("%s\n",result);
    }
    
}