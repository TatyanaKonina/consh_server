
#pragma comment(lib, "ws2_32.lib")

#define _CRT_SECURE_NO_WARNINGS
#define HAVE_STRUCT_TIMESPEC
#include <winsock.h>
#include <stdio.h>
#include <stdlib.h>
#include<pthread.h>
#include <signal.h>
#include<string.h>
#include <windows.h>

#define LENGTH 2048
#define CHATDATA 1024
#define IPADDR "127.0.0.1"
#define PORT 9000
#define NICK_LEN 20

void* do_send_chat(void* arg);

void* do_receive_chat(void* arg);

void str(char* arr, int length);

pthread_t thread_1, thread_2;
char first_mess[] = "Hello, dear Hacker! Who do you want to be today?\n1/Im bot.\n2.I aM nOt A bOt TrUsT mE";
char write_name[] = "Input name: ";
char dialog[CHATDATA] = "===Dialog with ";
char list[] = "===Here list of our clients:===\n";
char notification[] = " send you a message";
char update[] = "reload";
char    escape[] = "exit";
char    nickname[NICK_LEN];
int flag = 0;

int main(int argc, char* argv[])
{
    WSADATA wsd;
    if (WSAStartup(MAKEWORD(1, 1), &wsd) != 0)
    {
    	printf("Can't connect to socket lib");
    	return 1;
    }
    SOCKET c_socket;
    struct sockaddr_in c_addr;
    int len;
    char chatData[CHATDATA];
    char buf[CHATDATA];
    int nfds;
    fd_set read_fds;
    int n;
    c_socket = socket(PF_INET, SOCK_STREAM, 0);
    c_addr.sin_addr.S_un.S_addr = inet_addr(IPADDR);
    c_addr.sin_family = AF_INET;
    c_addr.sin_port = htons(PORT);
    if (connect(c_socket, (struct sockaddr*) & c_addr, sizeof(c_addr)) == -1) {
        printf("Can not connect\n");
        return -1;
    }
    memset(chatData, 0, sizeof(chatData));

    //здесь нужно вставить вариант с ботом
    //
    //
    //
    //

    printf(first_mess);
    fgets(nickname, NICK_LEN, stdin);
    fflush(stdin);
    str(nickname, NICK_LEN);
    //scanf("%s", nickname);

    send(c_socket, nickname, strlen(nickname),0);

    memset(chatData, 0, sizeof(chatData));
    recv(c_socket, chatData, sizeof(chatData), 0);

    printf("%s", chatData);// print welcome mess

    pthread_create(&thread_2, NULL, do_receive_chat, (void*)&c_socket);
    pthread_create(&thread_1, NULL, do_send_chat, (void*)&c_socket);

    pthread_join(thread_1, (void**)NULL);
    pthread_join(thread_2, (void**)NULL);
    closesocket(c_socket);
}

void * do_send_chat (void* arg){
    char chatData[CHATDATA];
    char buf[CHATDATA];
    //int n;
    SOCKET c_socket = *(SOCKET*)arg;
	while (1) {
        
        memset(chatData, '\0', sizeof(chatData));
		memset(buf, '\0', sizeof(buf));
        
        fgets(buf, CHATDATA, stdin);
        fflush(stdin);
        //scanf("%s", buf);
        
		str(buf, CHATDATA);

        sprintf(chatData, "%s: %s", nickname, buf);
        send(c_socket, chatData, strlen(chatData),0);
		
	}
   flag = 1;
}
void* do_receive_chat(void* arg)
{
    char    chatData[CHATDATA];
    int    n;
    SOCKET   c_socket = *(SOCKET*)arg;        // client socket
    while (1) {
        memset(chatData, 0, sizeof(chatData));
        if ((n = recv(c_socket, chatData, sizeof(chatData),0)) > 0) {
            if (strstr(chatData, dialog) != NULL || strstr(chatData,list)) {// client want to update screen
                system("cls");
            }
            printf("%s", chatData);
            //hourse head
            if (strstr(chatData, notification)) {
                Sleep(3000);
                for (int i = 0; i < strlen(chatData); i++) {//clean notification
                    printf("\b \b");
                }
                send(c_socket, update, sizeof(update), 0);
            }
        }
    }
}
void str (char* arr, int length) {
	int i;
	for (i = 0; i < length; i++) { // trim \n
		if (arr[i] == '\n') {
			arr[i] = '\0';
			break;
		}
	}
}
