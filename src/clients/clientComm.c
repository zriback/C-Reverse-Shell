#include "clientComm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

/*
Communicates with the given socket from the client side
Return values:
    0 - the connection should be ended
    1 - continue the connection
*/
int comm(SOCKET sock, int BUF_SIZE, int INPUT_SIZE){
    char buf[BUF_SIZE];
    char input[INPUT_SIZE];
        
    // prompt user
    printf("> ");
    fgets(input, sizeof(input), stdin);  // use fgets to input with spaces can be accepted
    input[strcspn(input, "\n")] = '\0';  // get rid of the trailing new line character
    // first check if the client wants to close the connection (by typing "close")
    if (strcmp(input, "close") == 0){
        return 0;
    }
    else{
        // create a smaller sending string first the is the size of the actual input
        char * sendString = (char*)calloc(strlen(input)+1, sizeof(char));
        strcpy(sendString, input);
        int sendResult = send(sock, sendString, strlen(input)+1, 0);
        // 0=text reply, 1=binary reply
        char * filename = (char*)calloc(64, sizeof(char));
        int replyType = processCmd(sendString, filename);
        free(sendString);

        if (sendResult != SOCKET_ERROR){
            // turns to true (1) once the end of the message is recieved (recieved the 0x03 ETX byte)
            if (replyType == 0){ // 0 for text/string
                recvString(sock, BUF_SIZE);
            }
            else{ // 1 for file
                recvFile(sock, filename, BUF_SIZE*2);
            }
            free(filename);
        }
        else{
            printf("Error sending command");
            return 0;
        } 
        printf("\n");
    }
    return 1;
}

// recieve reply as a string
void recvString(SOCKET sock, int BUF_SIZE){
    char buf[BUF_SIZE];
    int msgEnd = 0;
    while (!msgEnd){
        memset(buf, 0, sizeof(buf));
        int bytesRecv = recv(sock, buf, sizeof(buf), 0); // waits for response and blocks - response is copied into buff
        
        if (bytesRecv > 0){  // then print the message
            char * response = (char*)calloc(bytesRecv+1, sizeof(char));
            strncpy(response, buf, bytesRecv);

            // if the last byte (excluding the \0) is 0x03, this is the end of the message and we should end the loop
            if (*(response+strlen(response)-1) == 0x03){
                msgEnd = 1;
                // get rid of the ETX byte before printing
                *(response+strlen(response)-1) = '\0';
            }
            printf("%s", response);
            free(response);
        }
    } 
}

// recieve reply as binary
void recvFile(SOCKET sock, char * filename, int BUF_SIZE){
    char buf[BUF_SIZE];
    int msgEnd = 0;
    FILE *fp = fopen(filename, "wb");
    while(!msgEnd){
        memset(buf, 0, sizeof(buf));
        int bytesRecv = recv(sock, buf, sizeof(buf), 0); // waits for response and blocks - response is copied into buff
        Sleep(.65); // slow it down a bit maybe helps?
        int * bytes = (int*)buf;
        
        for(int i = 0; i < bytesRecv/sizeof(int); i++){
            if(*(bytes+i) == -1){ // the end byte
                msgEnd = 1;
                // printf("msg end\n");
                break;
            }
            fputc(*(bytes+i), fp);
        }
        // printf("after while\n");
    }
    fclose(fp);

}

/*
Processes the command
Return values indicated reply type
    0 - recieve text
    1 - recieve file (binary)
*/
int processCmd(char * cmd, char * filename){
    char cmdCpy[64];
    strcpy(cmdCpy, cmd);
    // the top level (first) command
    char * topCmd;
    topCmd = strtok(cmdCpy, " ");

    if (strcmp(topCmd, "transfer") == 0){
        strcpy(filename, strtok(NULL, "\0"));
        return 1;
    }
    else if (strcmp(topCmd, "screenshot") == 0){
        strcpy(filename, "capture.PNG");
        return 1;
    }
    else{
        return 0;
    }
}