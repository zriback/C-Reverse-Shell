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
        int sendResult = send(sock, input, sizeof(input), 0); //maybe need a size+1 here?
        if (sendResult != SOCKET_ERROR){
            memset(buf, 0, sizeof(buf));
            int bytesRec = recv(sock, buf, sizeof(buf), 0); // waits for response and blocks - response is copied into buff
            if (bytesRec > 0){  // then print the message
                char * response = (char*)calloc(bytesRec+1, sizeof(char));
                strncpy(response, buf, bytesRec);

                printf("%s", response);
            }
        }
        else{
            printf("Error sending command");
            return 0;
        }
        printf("\n");
    }
    
    return 1;
}