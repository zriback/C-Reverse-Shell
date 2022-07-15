#ifndef SHELL_COMM_HEADER
#define SHELL_COMM_HEADER

#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int comm(SOCKET sock, int BUF_SIZE, int REPLY_SIZE);

char* processMsg(char * msg, int REPLY_SIZE);

char* processCmd(char * cmd, int REPLY_SIZE);

#endif