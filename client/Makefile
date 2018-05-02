ftpserver: ftpserver.o server_help.o 
	gcc -o ftpserver ftpserver.o server_help.o
	
ftpclient: ftpclient.o client_help.o
	gcc -o ftpclient ftpclient.o client_help.o

server_help.o:server_help.c
	gcc -c server_help.c 

client_help.o:client_help.c
	gcc -c client_help.c

	
clean:
	clear ; rm *.o ; rm *.txt