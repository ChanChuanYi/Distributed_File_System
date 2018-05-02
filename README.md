# Project Title
A Distributed File Sharing System

## File List:

Makefile - simple Makefile. Execute with "make" for ftpserver and "make ftpclient" for ftpclient to compile the program
ftpserver.c - FTP server main program
server_help.c - several helper function that is used in the FTP server main program,
	                including file sending, file reading and file list sending.
ftpserver.c - FTP client main program                
client_help.c - several helper function that is used in the FTP client main program,
	                including file sending, file reading and file list reading.

## How to run:
Server: run with 
```
./ftpserver <port-num>
```
Client: run with 
```
"./ftpclient <IP-address> <port-num>"
```
## Issue:
	* Client:
	1. Client concurrency is not implemented. User can only enter next command after the previous command finishes.
	* Server:
	1.FTP server can accept multiple connection and the same time. However, it can only receive next command until the previous one is done,
	  which means "ABOR" cannot abort current file transmission.

