#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")



// Two macros for stringizing s
// More info at https://gcc.gnu.org/onlinedocs/cpp/Argument-Prescan.html#Argument-Prescan
#define xstr(s) str(s)
#define str(s) #s

int port;
char * ip;

// more networking stuff

int main(){
    port = 4444;  // default value
    ip = (char*)calloc(16, sizeof(char));
    strcpy(ip, "192.168.1.1");  // default value

    #ifdef PORT
        port = PORT;
    #endif

    #ifdef IP
        ip = xstr(IP);
    #endif
    
    printf("Port: %d\n", port);
    printf("IP: %s\n", ip);

    // startShell();

    free(ip);

    return 0;
}

/*
Starts the shell
Return values:
    0 - exited normally/successfully
    1 - exited with error
*/
int startShell() {
    // initialize winsock
    WSADATA wsData;
    WORD version = MAKEWORD(2,2);
    int wsock = WSAStartup(version, &wsData);
    if ( wsock != 0) {
        printf("Can't initialize winsock. Quitting...");
        return 1;
    }

    // create a socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET){
        printf("Invalid socket. Quitting...");
        return 1;
    }

    // bind socket to ip address and port
    struct sockaddr_in hint; // a hint structure (?)
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    hint.sin_addr.S_un.S_addr = INADDR_ANY; //same thing as the inet_pton line - tells to use any address
    bind(sock, (struct sockaddr*)&hint, sizeof(hint));

    // tell winsock the socker for meant to listen for incoming connections
    listen(sock, SOMAXCONN);

    // wait for a connection
    struct sockaddr_in client;
    int clientSize = sizeof(client);
    SOCKET clientSocket = accept(sock, (struct sockaddr*)&client, &clientSize);

    // the connected host's ip
    char client_ip[NI_MAXHOST];
    memset(client_ip, "\0", sizeof(client_ip));

    inet_ntop(AF_INET, &client.sin_addr, client_ip, NI_MAXHOST);
    printf("%s connected on port __(fill this in)", client_ip);

    for (;;){
        
    }
}