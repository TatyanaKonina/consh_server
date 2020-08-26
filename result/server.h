#ifndef SERVER_H
#define SERVER_H

#define MAX_LEN 20
#define MAX_CLIENT 5
#define CHATDATA 1024
#define INVALID_SOCK -1
#define PORT 9000

typedef struct guest {
	char nickname[MAX_LEN];
	char password[MAX_LEN];
	char history[CHATDATA][CHATDATA];
	int last_mess;
	SOCKET socket;
} guest;

typedef struct socket_info {
	int roomNum;
	SOCKET socket;
} socket_info;

extern int cli_count;
extern int lastChatMess;

void* do_chat(void* arg);
int pushClient(int roomNum, SOCKET c_socket, char* nickname);
void* printClients(SOCKET soket_client);
int find_mate_box(char* name);
void str(char* arr, int length);
void* print_history(int room_mate, int room_client);
void* printChatHistory(SOCKET client);

#endif#pragma once
