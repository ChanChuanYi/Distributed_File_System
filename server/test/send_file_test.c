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
#include <time.h>
#include "server_help.h"

int main(int argc, char **argv)
{
    if (argc != 2){
        printf("usage: a.out <port-num>"); 
        exit(0);
    }
    int listenfd = 0,n,filesize=0;
    int connfd = 0,data_socket=0;
    int port=0;
    struct sockaddr_in serv_addr,client_address;
    int n1,n2,n3,n4,n5,n6;
    char sendBuff[LINE],PORT[LINE];
    
    
    char IP[LINE];
    char flag;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    printf("Socket retrieve success\n");

    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr));



    if(listen(listenfd, 10) == -1)
    {
        printf("Failed to listen\n");
        return -1;
    }
    while(1)
    {
        
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
        printf("%s\n","connect success");

        int child =fork();
        if(child==0){
            close(listenfd);
            char command[10],filename[LINE],directory[LINE];
            char recv[LINE];
            bzero(recv,sizeof(recv));
            bzero(command,sizeof(command));
            bzero(filename,sizeof(filename));
            bzero(directory,sizeof(directory));

            read(connfd,PORT,LINE); //receive data process PORT information
            
            sscanf(PORT,"%*[^ ] %d,%d,%d,%d,%d,%d",&n1,&n2,&n3,&n4,&n5,&n6);
            snprintf(IP,sizeof(IP),"%d.%d.%d.%d",n1,n2,n3,n4);
            port = 256*n5+n6;
            printf("IP:%s PORT:%d\n",IP,port);
            printf("%s\n","-----------");

            while(read(connfd,recv,LINE) > 0){
                
                printf("%s\n",recv);
                sscanf(recv,"%s %s %s",command,filename,directory);
                if(strncmp("RETR",command,strlen(command))==0){

                    data_socket=create_data_socket(IP,port);
                    send_file(connfd,data_socket,filename);
                    close(data_socket);

                }
                
                printf("%s\n","-------------------------");
                
            }
            printf("%s\n","connection close");
            close(connfd);
            exit(0);
            break;
        }    
        close(connfd);
           
    }
       
        
      
    return 0 ;
}