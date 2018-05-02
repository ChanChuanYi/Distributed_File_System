#define LINE 1024
#define FILE_LENGTH 8196
#define FILE_TRANSFER_LENGTH 5000
int get_filesize(FILE *fp);

int create_data_socket(char* IP, int PORT);

void read_file(int commandfd, int datafd, char* filename);

void send_file(int commandfd, int datafd, char* filename);

void List_Direct(int commandfd,int datafd, char* filename,char* directory);

