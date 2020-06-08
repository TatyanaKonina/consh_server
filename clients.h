#ifndef CLIENTS_H
#define CLIENTS_H

#define LENGTH 2048
#define CHATDATA 1024
#define IPADDR "127.0.0.1"
#define PORT 9000
#define NICK_LEN 20
#define MAX_CLIENT 5

#define MID 177
#define LIG 178
#define DAR 176
#define RIG 221
#define LEF 222
#define DOW 220
#define UP 223
#define X 55
#define HEIGHT 12
#define WIDTH 23

extern char nickname[NICK_LEN];
extern int flag_bot;
extern const char pony[HEIGHT][WIDTH];

void* doSendChat(void* arg);
void* do_receive_chat(void* arg);
char** read_data(char* file_name, int line_num);
void str(char* arr, int length);
int words_num_in_file(char* file_name);
void printPony();

#endif