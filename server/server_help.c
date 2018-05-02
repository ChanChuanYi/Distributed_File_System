#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include "server_help.h"

int get_filesize(FILE *fp){
    int prev=ftell(fp);
    fseek(fp, 0L, SEEK_END);
    int sz=ftell(fp);
    fseek(fp,prev,SEEK_SET); //go back to where we were
    return sz;
}


int create_data_socket(char* IP, int PORT){
	struct sockaddr_in	serv_addr;
	int data_sock;
	int n=-1;
	if((data_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("%s\n","error creating socket");
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port   = htons(PORT);	/* daytime server */
	serv_addr.sin_addr.s_addr = inet_addr(IP);

	if((n =connect(data_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr))) < 0){
		printf("%s\n","Connection Failed");
		
	}
	
	return data_sock;
}

void read_file(int commandfd, int datafd, char* filename){
	char file_buf[FILE_LENGTH];
    char reply[LINE];
    char flag;
    int n,m;
    bzero(file_buf,sizeof(file_buf));
    printf("%s\n",reply);
    read(datafd,&flag,1);
    if(strncmp(&flag,"0",1)==0){
    	FILE *recv_fp=fopen(filename,"w+");
        while((n=read(datafd, file_buf, FILE_TRANSFER_LENGTH))>0){
            printf("receiver size:%lu\n",strlen(file_buf));
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
	char file_buf[FILE_LENGTH];
	char reply[LINE];
	int count=0,start=0,tail=0,bread=0;
	send_fp = fopen(filename,"rb");
	memset(file_buf, '\0', sizeof(file_buf));
	
	if(send_fp==NULL){
		snprintf(reply,sizeof(reply),"450 %s: No such file or directory",filename);
		write(commandfd,reply,strlen(reply));
		write(datafd,"1",1);
	}
	else{
		snprintf(reply,sizeof(reply),"200 Command OK");
		write(commandfd,reply,strlen(reply));
		write(datafd,"0",1);

		filesize=get_filesize(send_fp); //get file size for "fread"
		count = filesize/FILE_TRANSFER_LENGTH;
		tail= filesize%FILE_TRANSFER_LENGTH;
		printf("%d\n", filesize);
		//If file size if too big, we need to send with several fragment
		for(int i =0;i<count;i++){
			fseek(send_fp,start,SEEK_SET);
			bread = fread(file_buf,1,FILE_TRANSFER_LENGTH,send_fp);//read content from file
			write(datafd,file_buf,strlen(file_buf));//write to destination
			printf("%d\n",bread);
			start=FILE_TRANSFER_LENGTH*(i+1);
			memset(file_buf, '\0', sizeof(file_buf));
		}

		//send the rest
		fseek(send_fp,start,SEEK_SET);
		bread = fread(file_buf,1,tail,send_fp);//read content from file
		printf("%d\n",bread);
		write(datafd,file_buf,strlen(file_buf));//write to destination
		memset(file_buf, '\0', sizeof(file_buf));
	}
	fclose(send_fp);	
}


void List_Direct(int commandfd,int datafd, char* filename,char* directory){
	FILE *lsfp;
    char arg[LINE];
    char result[LINE],sendBuff[LINE],reply[LINE];
   
    bzero(result, sizeof(result));
    bzero(sendBuff, sizeof(sendBuff));
    snprintf(arg,sizeof(arg),"ls %s %s",filename,directory);
    lsfp = popen(arg,"r");
    while (fgets(result, LINE, lsfp) != NULL){
            strncat(sendBuff, result,strlen(result));
        }
    if(strlen(sendBuff)==0){
        snprintf(reply,sizeof(reply),"450 %s %s: No such file or directory",filename,directory);
      	write(commandfd,reply,strlen(reply));
      	write(datafd,"1",1);
      	write(datafd,sendBuff,strlen(sendBuff));
    }
    else{
    	snprintf(reply,sizeof(reply),"200 Command OK");
    	write(commandfd,reply,strlen(reply));
    	write(datafd,"0",1);
        write(datafd,sendBuff,strlen(sendBuff));
    }
    pclose(lsfp);     

}