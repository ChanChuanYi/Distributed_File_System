#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "client_help.h"


int main(int argc, char **argv)
{
	if (argc != 3){
        printf("usage: a.out <IPaddress> <Port Number>"); 
        exit(0);
    }
	int sockfd, n,m,data_connfd=0,datafd;
	
	struct sockaddr_in	serv_addr;
	char test[]="quit",recv[LINE],buf[LINE];
	char command[10],filename[LINE],directory[LINE];
	char IP[NI_MAXHOST]; //have to change to external IP
	int n1=0,n2=0,n3=0,n4=0,n5=0,n6=0;
	
	n5=FTP_PORT/256;
	n6=FTP_PORT%256;
	//get_client_IP(IP);
	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("%s\n","error creating socket");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port   = htons(atoi(argv[2]));	/* daytime server */
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);

	get_client_IP(IP);//get local IP
	printf("%s\n",IP);
	sscanf(IP,"%d.%d.%d.%d",&n1,&n2,&n3,&n4); //Parse IP address

	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		printf("%s\n","Error: Connection Failed");
	}
	snprintf(buf,sizeof(buf),"PORT %d,%d,%d,%d,%d,%d",n1,n2,n3,n4,n5,n6);
	printf("%s\n",buf);
	write(sockfd,buf,strlen(buf));//send PORT command right after connection



	datafd = create_data_socket(FTP_PORT);//set up data process socket first

	while(1){
		memset(buf,'\0', sizeof(buf));
		printf("Please enter: <Command> (<Filename>)\n");
	
		
				//capture user input
		
		fgets(buf, LINE, stdin);
		if(strncmp(buf,test,4)==0){
			//abort current action
			char reply[LINE];
			int n=0;
			snprintf(buf,sizeof(buf),"ABOR");
			write(sockfd,buf,strlen(buf));
			if((n=read(sockfd,reply,LINE))>0){
				reply[n]=0;
			}
			printf("%s\n",reply);
			break;
		}
		else{
			m= sscanf(buf,"%s %s %s",command,filename,directory);
			
	
			//see what the method is
			if(strncmp("get",command,strlen(command))==0){    //request file from server

				snprintf(buf,sizeof(buf),"RETR %s",filename);
				write(sockfd,buf,strlen(buf));
				
				data_connfd = accept(datafd, (struct sockaddr*)NULL ,NULL);
				
				if(data_connfd<0){
					printf("%s\n","425 Can't open data connection");
					continue;
				}

				printf("%s\n","------Connection Succeed, Starting Data Process------");
				read_file(sockfd,data_connfd,filename);
				close(data_connfd);
				
			}
			else if(strncmp("put",command,strlen(command))==0){  //send file to server
	
				snprintf(buf,sizeof(buf),"STOR %s",filename);
				write(sockfd,buf,strlen(buf));
				
				data_connfd = accept(datafd, (struct sockaddr*)NULL ,NULL);
				
				if(data_connfd<0){
					printf("%s\n","425 Can't open data connection");
					continue;
				}
				send_file(sockfd,data_connfd,filename);
				printf("%s\n","Connection Succeed, Starting Data Process");
				close(data_connfd);
			}
			else if(strncmp("ls",command,strlen(command))==0){
				snprintf(buf,sizeof(buf),"LIST %s %s",filename,directory);
				write(sockfd,buf,strlen(buf));
				data_connfd = accept(datafd, (struct sockaddr*)NULL ,NULL);
				
				if(data_connfd<0){
					printf("%s\n","425 Can't open data connection");
					continue;
				}
				read_ls(sockfd,data_connfd);
				close(data_connfd);
			}
			else{
				printf("%s\n","500 Syntax error: Unrecognized Command");
				continue;
			}
			printf("%s\n","-------------------------");
			
		}


		
	
	}
			

	exit(0);
}
