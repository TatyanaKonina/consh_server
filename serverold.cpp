#define _CRT_SECURE_NO_WARNINGS
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>
#pragma comment(lib, "ws2_32.lib")
#include <winsock.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "server.h"

pthread_mutex_t mutex;

int cli_count = 0;
guest list_c[MAX_CLIENT][MAX_CLIENT];

int main(int argc, char* argv[])
{
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(1, 1), &wsd) == 0)
	{
		printf("Connected to socket lib\n");
	}
	SOCKET c_socket, s_socket;
	socket_info si;
	struct sockaddr_in localaddr, clientaddr;
	pthread_t thread;
	int    len;
	int    i, j, n;
	int    res;
	char nickname[20];
	char password[MAX_LEN];
	char greeting[] = "=== WELCOME TO THE CHATROOM ===\n";
	char denied[] = "Sorry, assess denied\n";
	char roomNum[3]; int r_Num;
	if (pthread_mutex_init(&mutex, NULL) != 0) {
		printf("Can not create mutex\n");
		return -1;
	}
	s_socket = socket(PF_INET, SOCK_STREAM, 0);

	localaddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	localaddr.sin_family = AF_INET;
	localaddr.sin_port = htons(PORT);//port number is for example, must be more than 1024
	;
	if (bind(s_socket, (struct sockaddr*) & localaddr, sizeof(localaddr)) == SOCKET_ERROR) {
		printf("Can not Bind\n");
		return -1;
	}
	if (listen(s_socket, MAX_CLIENT) == -1) {
		printf("listen Fail\n");
		return -1;
	}
	for (i = 0; i < MAX_CLIENT; i++) {//CLEAN ROOMS 
		for (j = 0; j < MAX_CLIENT; j++) {
			list_c[i][j].last_mess = 0;
			list_c[i][j].socket = INVALID_SOCK;
			strcpy(list_c[i][j].nickname, "\0");
		}
	}
	while (1) {

		len = sizeof(clientaddr);
		c_socket = accept(s_socket, (struct sockaddr*) & clientaddr, &len);
		cli_count++;

		memset(nickname, 0, sizeof(nickname));//clean func
		recv(c_socket, nickname, sizeof(nickname), 0);// write name

		memset(password, 0, sizeof(password));
		recv(c_socket, password, sizeof(password), 0);
		//printf("%s %s\n", nickname, password);

		int res = pushClient(cli_count - 1, c_socket, nickname);// push socket into room box
		if (res < 0) { //
			send(c_socket, denied, strlen(denied), 0);
			closesocket(c_socket);
		}
		else {
			printf(" name = %s, passowrd = %s, cli_cpount = %d\n", nickname, password, cli_count);
			send(c_socket, greeting, sizeof(greeting), 0);//welcome mess
			Sleep(100);
			pthread_create(&thread, NULL, do_chat, (void*)(cli_count - 1));
		}
	}
}

void* printClients(SOCKET soket_client) {
	char mess[CHATDATA];
	for (int i = 0; i < MAX_CLIENT; i++) {
		if (soket_client != list_c[i][0].socket && list_c[i][0].socket != INVALID_SOCK) {//print all except client
			strcpy(mess, list_c[i][0].nickname);
			strcat(mess, "\n");
			send(soket_client, mess, sizeof(mess), 0);
		}
	}
	return (void*)0;
}


int find_mate_box(char* name) {
	//return i if nick_name exist 
	//return -1 if no match 
	char* istr;
	istr = strtok(name, ":");
	istr = strtok(NULL, "\0");
	for (int i = 0; i < MAX_CLIENT; i++) {
		if (strstr(istr, list_c[i][0].nickname)) {
			return i;
		}
	}
	return -1;
}

void* print_history(int room_mate, int room_client) {

	int lst_mess = list_c[room_client][room_mate].last_mess;
	char clear_history[] = "Sorry, no previous messages. Start chattering right now\n";
	if (lst_mess == 0) {
		send(list_c[room_client][0].socket, clear_history, sizeof(clear_history), 0);// no history print mess
	}
	for (int i = 0; i < lst_mess; i++) {
		send(list_c[room_client][0].socket, list_c[room_client][room_mate].history[i], sizeof(list_c[room_client][room_mate].history[i]), 0);
	}
	return (void*)0;
}

void str(char* arr, int length) {
	int i;
	for (i = 0; i < length; i++) { // trim \n
		if (arr[i] == '\n') {
			arr[i] = '\0';
			break;
		}
	}
}

void* do_chat(void* arg)
{
	int room = (int)arg;

	SOCKET c_socket = list_c[room][0].socket;

	char mate_name[MAX_LEN];
	char chatData[CHATDATA];
	char buffer[CHATDATA];
	char escape[] = "exit";
	char start_mess[] = "View dialog with ->";
	char neUser[] = "This is Not User\n";
	char update[] = "reload";
	char notification[] = " send you a message\n";
	char one_client[] = "===Your are alone, don't worry, please,wait===\n";
	char list[] = "===Here list of our clients:===\n";
	char end[] = "--------------------------------------------\n";

	int i = 0, j = 0, n = 0;
	while (1) {
		if (cli_count != 1) {
			memset(mate_name, 0, sizeof(mate_name));

			send(c_socket, list, sizeof(list), 0);
			Sleep(100);//we need pause to clean console
			printClients(c_socket);//print all clients except user
			send(c_socket, start_mess, sizeof(start_mess), 0);//send "view dialog with "
			recv(c_socket, mate_name, sizeof(mate_name), 0);//recv name
			if (strstr(mate_name, escape) == NULL) {// if user do not want to leave

				int mate_num_box = find_mate_box(mate_name);// res = number of mate_box

				if (mate_num_box >= 0) {//if we found mate
					i = pushClient(room, c_socket, list_c[mate_num_box][0].nickname);
					j = pushClient(mate_num_box, c_socket, list_c[room][0].nickname);

					if ((i != -1) && (j != -1)) {//if succesfully push both clients
						//start chat
						memset(chatData, 0, sizeof(chatData));

						while (1) {
							//print history
							memset(buffer, 0, sizeof(buffer));//dialog icon
							strcpy(buffer, "===Dialog with ");
							strcat(buffer, list_c[mate_num_box][0].nickname);
							strcat(buffer, "===\n");

							send(c_socket, buffer, sizeof(buffer), 0);
							//Sleep(100);//because we need time to clean console 
							memset(chatData, 0, sizeof(chatData));

							print_history(i, room);//print previous mess
							//Sleep(100);
							send(c_socket, end, sizeof(end), 0);
							n = recv(c_socket, chatData, sizeof(chatData), 0);

							//3 options

							if (strstr(chatData, escape)) {//if user want to leave
								break;
							}
							else if (strstr(chatData, update)) {
								continue;
							}
							else {// if user send a mess
								struct tm* u;//define time
								const time_t timer = time(NULL);
								u = localtime(&timer);
								char s1[CHATDATA] = { 0 };
								strftime(s1, CHATDATA, "[ %H : %M ] ", u);
								strcat(s1, chatData);
								strcat(s1, "\n");

								strcpy(list_c[room][i].history[list_c[room][i].last_mess], s1);//save mess in user history
								list_c[room][i].last_mess++;
								strcpy(list_c[mate_num_box][j].history[list_c[mate_num_box][j].last_mess], s1);//save mess in mate history
								list_c[mate_num_box][j].last_mess++;
								memset(chatData, 0, sizeof(chatData));
								strcpy(chatData, list_c[room][0].nickname);
								strcat(chatData, notification);
								pthread_mutex_lock(&mutex);
								send(list_c[mate_num_box][0].socket, chatData, sizeof(chatData), 0);//send notification about new mess to mate
								pthread_mutex_unlock(&mutex);

							}
						}

					}
				}
				else {
					send(c_socket, neUser, sizeof(neUser), 0);//wrong name
				}
			}
			else {
				send(c_socket, escape, sizeof(escape), 0);
				closesocket(c_socket);
				break;
			}
		}
		else {
			send(c_socket, one_client, sizeof(one_client), 0);//please wait mess
			Sleep(10000);
		}
	}
	return (void*)0;
}

int pushClient(int roomNum, SOCKET c_socket, char* nickname) {
	//ADD c_socket to list_c array.
	//
	///////////////////////////////
	//return -1, if too much clients
	//return the index of list_c which c_socket is added.
	int i;
	for (i = 0; i < MAX_CLIENT; i++) {//puch into free room
		if (list_c[roomNum][i].socket == INVALID_SOCK || (strstr(list_c[roomNum][i].nickname, nickname))) {
			//if dialog exist or we found free place
			list_c[roomNum][i].socket = c_socket;
			strcpy(list_c[roomNum][i].nickname, nickname);
			break;
		}
	}
	if (i == MAX_CLIENT || cli_count == MAX_CLIENT) {
		i = -1;
	}
	return i; // list_c index.
}