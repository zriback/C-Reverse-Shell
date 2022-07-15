#include "shellComm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>

// Need to link with Ws2_32.lib
// unfortunately this does not work, so when compiling we manually tell gcc to use this library to compile with the "-lws2_32" option.
#pragma comment (lib, "ws2_32.lib")

/*
Communicates with the given socket from the server/shell side
Return values:
    0 - the connection should be ended
    1 - continue the connection
*/
int comm(SOCKET sock, int BUF_SIZE, int REPLY_SIZE){
    char buf[BUF_SIZE];     // space for reply from client
    char * reply;           // space for message to client
    
    
    memset(buf, 0, sizeof(buf));
    int bytesRecv = recv(sock, buf, sizeof(buf), 0); // this function returns the length of the message in bytes
    if (bytesRecv == SOCKET_ERROR){
        printf("Error recieving data. Exiting...\n");
        return 0;
    } else if (bytesRecv == 0){  // may not need this?
        printf("Client disconnected. Exiting...\n");
        return 0;
    }

    // at this point we have there message in buf and the size in bytesRecv
    if (strcmp(buf, "close") != 0){
        reply = processMsg(buf, REPLY_SIZE);

        // send message back to the client
        send(sock, reply, REPLY_SIZE, 0);
        free(reply);
        return 1;
    }
    else{  // close was sent
        printf("Connection was closed by client.");
        return 0;
    }   
}

/*
Function for proccessing the input sent from the client
Places the necessary response in char *reply, and returns a pointer to the reply in memory
*/
char* processMsg(char * msg, int REPLY_SIZE){
    // check if a shell (cmd prompt) command is being issued
    // an input that starts with "cmd" or "c"
    if (strcmp(strtok(msg, " "), "cmd") == 0 || strcmp(strtok(msg, " "), "c") == 0){
        msg+=(strcspn(msg, " ")+1); // pass everthing after the "cmd " or "c " at the beginning of the msg
        return processCmd(msg, REPLY_SIZE);
    }
    else {  // it is a command not meant to be executed by the shell
        char * reply = (char*)calloc(REPLY_SIZE, sizeof(char));
        strcpy(reply, "That is not a shell command and is currently not supported\n");
        return reply;
    }
}

char* processCmd(char * cmd, int REPLY_SIZE) {
    FILE *fp;

    // may be problem here with buffer overflow causing crash when reply is too big
    fp = popen(cmd, "r");
    char * finalResult = (char*)calloc(REPLY_SIZE, sizeof(char));
    char line[REPLY_SIZE];

    int bytesCat = 0;   // keeps track of the # of bytes concatenated to prevent buffer overflow 
    while (fgets(line, REPLY_SIZE, fp) != NULL) {
        bytesCat+=(strlen(line));
        strncat(finalResult, line, REPLY_SIZE-bytesCat);
        if (bytesCat >= REPLY_SIZE){
            break;
        }
    }

    return finalResult;
}