#pragma comment(lib, "ws2_32.lib")

#define _CRT_SECURE_NO_WARNINGS
#define HAVE_STRUCT_TIMESPEC
#include <winsock.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <windows.h>
#include "clients.h"

char nickname[NICK_LEN];
int flag_bot = 0;

const char pony[HEIGHT][WIDTH] = { {MID,MID,MID,MID,LIG,MID,MID,MID,MID,MID,MID,MID,MID,MID,MID,MID,MID,MID,MID,DAR,DAR,DAR,'\0'},
                            {MID,MID,MID,LIG,LIG,LIG,MID,MID,MID,MID,MID,MID,MID,MID,MID,MID,MID,MID,MID,MID,MID,DAR,'\0'},
                            {MID,MID,LIG,RIG,LEF,LIG,MID,LIG,LIG,LIG,LIG,LIG,LIG,MID,MID,MID,MID,MID,MID,MID,MID,MID,'\0'},
                            {MID,MID,LIG,LIG,LEF,LIG,LIG,LIG,LIG,LIG,LIG,LIG,LIG,LIG,LIG,MID,MID,MID,MID,MID,MID,MID,'\0'},
                            {MID,MID,MID,MID,LIG,LIG,LIG,LIG,LIG,LIG,LIG,LIG,LIG,LIG,LIG,LIG,DAR,DAR,DAR,MID,MID,MID,'\0'},
                            {MID,MID,MID,MID,MID,MID,LIG,LIG,DOW,DOW,' ',' ',' ',LIG,LIG,LIG,LIG,DAR,MID,MID,MID,DAR,'\0'},
                            {MID,MID,MID,MID,MID,MID,LIG,LEF,UP,' ',' ',' ',' ',LEF,LIG,LIG,LIG,DAR,MID,MID,MID,DAR,'\0'},
                            {MID,MID,MID,MID,MID,LIG,LIG,' ',' ',' ',UP,' ',LEF,LIG,LIG,LIG,DAR,DAR,MID,DAR,DAR,DAR,'\0'},
                            {MID,MID,MID,MID,MID,MID,MID,LIG,DOW,DOW,DOW,DOW,LIG,LIG,LIG,LIG,DAR,DAR,MID,MID,DAR,DAR,'\0'},
                            {MID,MID,MID,MID,MID,MID,MID,MID,LIG,LIG,LIG,LIG,LIG,LIG,LIG,LIG,LIG,DAR,DAR,DAR,DAR,DAR,'\0'},
                            {MID,MID,MID,MID,MID,MID,MID,LIG,LIG,LIG,LIG,LIG,LIG,RIG,UP,LIG,DAR,DAR,DAR,DAR,DAR,DAR,'\0'},
                            {MID,MID,MID,MID,MID,MID,LIG,LIG,DAR,DAR,DAR,DAR,DAR,DAR,DAR,DAR,DAR,DAR,DAR,DAR,DAR,DAR,'\0'} };



int main(int argc, char* argv[])
{
    WSADATA wsd;
    if (WSAStartup(MAKEWORD(1, 1), &wsd) != 0)
    {
        printf("Can't connect to socket lib");
        return 1;
    }
    pthread_t thread_1, thread_2;
    SOCKET c_socket;
    struct sockaddr_in c_addr;
    int len;
    char chatData[CHATDATA];
    char buf[CHATDATA];
    int nfds;
    fd_set read_fds;
    int n;
    char password[NICK_LEN];
    c_socket = socket(PF_INET, SOCK_STREAM, 0);
    c_addr.sin_addr.S_un.S_addr = inet_addr(IPADDR);
    c_addr.sin_family = AF_INET;
    c_addr.sin_port = htons(PORT);
    if (connect(c_socket, (struct sockaddr*) & c_addr, sizeof(c_addr)) == -1) {
        printf("Can not connect\n");
        return -1;
    }
    memset(chatData, 0, sizeof(chatData));
    memset(nickname, 0, sizeof(nickname));
    memset(password, 0, sizeof(password));
    char file_name[] = "bot_names.txt";
    char passwords[] = "bot_passwords.txt";
    printPony();

    printf("=== WELCOME TO MY LIITLE SERVER ===\n");

    printf("Hello, dear Hacker! Who do you want to be today?\n1.Im bot.\n2.I aM nOt A bOt TrUsT mE\n");

    fgets(chatData, CHATDATA, stdin);
    fflush(stdin);

    str(chatData, CHATDATA);

    if (atoi(chatData) == 1) {
        srand(time(NULL));
        int word_line = words_num_in_file(file_name);
        char** data = read_data(file_name, word_line);
        int random = rand() % (word_line);
        strcpy(nickname, data[random]);
        word_line = words_num_in_file(passwords);
        data = read_data(passwords, word_line);
        random = rand() % (word_line);
        strcpy(password, data[random]);
        flag_bot = 1;
    }
    else {
        printf("Input name: ");
        fgets(nickname, NICK_LEN, stdin);
        fflush(stdin);
        printf("Write password: ");
        fgets(password, NICK_LEN, stdin);
        fflush(stdin);
    }
    //printf("%s %s", password, nickname);
    str(nickname, strlen(nickname));
    str(password, strlen(password));

    send(c_socket, nickname, strlen(nickname), 0);//send mess and password
    send(c_socket, password, strlen(password), 0);

    pthread_create(&thread_2, NULL, do_receive_chat, (void*)&c_socket);
    pthread_create(&thread_1, NULL, doSendChat, (void*)&c_socket);

    pthread_join(thread_1, (void**)NULL);
    pthread_join(thread_2, (void**)NULL);
    closesocket(c_socket);
}

void* doSendChat(void* arg) {
    char chatData[CHATDATA];
    char buf[CHATDATA];
    char escape[] = "exit";
    //int n;
    SOCKET c_socket = *(SOCKET*)arg;
    while (1) {

        memset(chatData, '\0', sizeof(chatData));
        memset(buf, '\0', sizeof(buf));
        if (flag_bot == 1) {
            fgets(buf, CHATDATA, stdin);
            fflush(stdin);
            str(buf, CHATDATA);
            sprintf(chatData, "%s: %s", nickname, buf);
            send(c_socket, chatData, strlen(chatData), 0);
            char file_name[] = "bot_phrase.txt";
            int word_line = words_num_in_file(file_name);
            char** data = read_data(file_name, word_line);
            int random = 0;
            srand(time(0));

            for (int i = 0; i < (3 + rand() % (MAXMESNUM - 3)); i++) {
                sprintf(buf, data[rand() % word_line]);
                str(buf, CHATDATA);
                sprintf(chatData, "%s: %s", nickname, buf);
                send(c_socket, chatData, strlen(chatData), 0);
                Sleep(100 + rand() % (MAXSLEEP - 100));
            }
            memset(chatData, '\0', sizeof(chatData));
            sprintf(chatData, "%s: %s", nickname, escape);
            send(c_socket, chatData, strlen(chatData), 0);
        }
        else {
            fgets(buf, CHATDATA, stdin);
            fflush(stdin);
            str(buf, CHATDATA);
            sprintf(chatData, "%s: %s", nickname, buf);
            send(c_socket, chatData, strlen(chatData), 0);
        }

    }
    //flag = 1;
}
void* do_receive_chat(void* arg)
{
    char    chatData[CHATDATA];
    int    n;
    char dialog[CHATDATA] = "===Dialog with ";
    char list[] = "Here list of our clients";
    char notification[] = " send you a message";
    char update[] = "reload";
    SOCKET   c_socket = *(SOCKET*)arg;        // client socket
    while (1) {
        memset(chatData, 0, sizeof(chatData));
        if ((n = recv(c_socket, chatData, sizeof(chatData), 0)) > 0) {
            if (strstr(chatData, dialog) != NULL || strstr(chatData, list)) {// client want to update screen
                system("cls");
                Sleep(100);
                printPony();
            }

            printf("%s", chatData);

            if (strstr(chatData, notification)) {
                Sleep(3000);
                for (int i = 0; i < strlen(chatData); i++) {//clean notification
                    printf("\b \b");
                }
                send(c_socket, update, sizeof(update), 0);
            }
            //printPony();
        }
    }
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

char** read_data(char* file_name, int line_num) {
    char** data = (char**)calloc(line_num, sizeof(char*));
    for (int i = 0; i < line_num; i++) {
        data[i] = (char*)calloc((NICK_LEN), sizeof(char));
    }
    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        exit(-7);
    }
    int pointer = 0;
    while (!feof(file)) {
        fgets(data[pointer], 100, file);
        data[pointer][strlen(data[pointer]) - 1] = '\0';
        pointer++;
    }
    fclose(file);
    return data;
}

int words_num_in_file(char* file_name) {

    int line_num = 0, any;
    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        return -7;
    }
    do {
        any = fgetc(file);
        if (any == '\n') {
            line_num++;
        }
    } while (any != EOF);

    line_num++;

    fclose(file);

    return line_num;
}

void printPony()
{
    HANDLE nConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    int i = 0;

    for (i = 0; i < HEIGHT; i++) {
        COORD coord = { X, i };
        SetConsoleCursorPosition(nConsole, coord);
        printf("%s", (pony[i]));
    }
    COORD coord = { 0, 0 };
    SetConsoleCursorPosition(nConsole, coord);

    return;
}