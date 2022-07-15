#ifndef CLIENT_COMM_HEADER
#define CLIENT_COMM_HEADER


#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int comm(SOCKET sock, int BUF_SIZE, int INPUT_SIZE);

#endif