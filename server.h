#ifndef SERVER_H
#define SERVER_H

#define _CRT_SECURE_NO_WARNINGS
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>
#pragma comment(lib, "ws2_32.lib")
#include <winsock.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

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

void* do_chat(void* arg);
int pushClient(int roomNum, SOCKET c_socket, char* nickname);
void* printClients(SOCKET soket_client);
int find_mate_box(char* name);
void str(char* arr, int length);
void* print_history(int room_mate, int room_client);

#endif