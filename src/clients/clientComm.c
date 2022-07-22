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
        int sendResult = send(sock, sendString, strlen(input)+1, 0); //maybe need a size+1 here?
        free(sendString);

        if (sendResult != SOCKET_ERROR){
            // turns to true (1) once the end of the message is recieved (recieved the 0x03 ETX byte)
            int msgEnd = 0;
            while (!msgEnd){
                memset(buf, 0, sizeof(buf));
                int bytesRec = recv(sock, buf, sizeof(buf), 0); // waits for response and blocks - response is copied into buff
                
                if (bytesRec > 0){  // then print the message
                    char * response = (char*)calloc(bytesRec+1, sizeof(char));
                    strncpy(response, buf, bytesRec);

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
        else{
            printf("Error sending command");
            return 0;
        } 
        printf("\n");
    }
    
    return 1;
}