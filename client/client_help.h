#define LINE 1024
#define FTP_PORT 3000 //define FTP PORT that is used for data process
#define FILE_LENGTH 8196
#define FILE_TRANSFER_LENGTH 5000
int create_data_socket(int PORT);//create socket for data transfer process

void get_client_IP(char* IP);//acquire client local IP

void read_file(int commandfd, int datafd, char* filename);//get file from server

void send_file(int commandfd, int datafd, char* filename);//send file to server

int get_filesize(FILE *fp);
//get content size of a file in order to send file to server

void read_ls(int commandfd, int datafd);