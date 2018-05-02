# Project Title
A Distributed File Sharing System

## File List:

* Makefile - simple Makefile. Execute with "make" for ftpserver and "make ftpclient" for ftpclient to compile the program
* ftpserver.c - FTP server main program
* server_help.c - several helper function that is used in the FTP server main program, including file sending, file reading and file list sending.
* ftpserver.c - FTP client main program                
* client_help.c - several helper function that is used in the FTP client main program,
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
## FTP Interface:
The client code start and gives a user interface prompt waiting for user commands.
Commands include:
* List current directory or file name
```
ls [<filename>]
```
* Send file to server
```
put <filename>
```
* Get file from server
```
get <filename>
```
* Exit the user interface
```
quit
```

## Issues:
* Client:
Client concurrency is not implemented. User can only enter next command after the previous command finishes.
* Server:
FTP server can accept multiple connection a the same time. However, it can only receive next command until the previous one is done, which means "ABOR" cannot abort current file transmission.

