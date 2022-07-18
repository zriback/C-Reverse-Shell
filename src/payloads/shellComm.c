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
int comm(SOCKET sock, int BUF_SIZE, int REPLY_MAX_SIZE){
    
    char buf[BUF_SIZE];     // space for reply from client
    char * reply;           // space for message to client

    // default working directory
    char * cwd;
    cwd = getCmdOut("echo %HOMEDRIVE%%HOMEPATH%", 128);
    cwd[strcspn(cwd, "\n")] = '\0';
    // NOTE: this code lead to breaking stuff if the program is run in a drive different then the home drive!!!!

    while (TRUE){
        memset(buf, 0, sizeof(buf));
        int bytesRecv = recv(sock, buf, sizeof(buf), 0); // this function returns the length of the message in bytes
        if (bytesRecv == SOCKET_ERROR){
            printf("Error recieving data. Exiting...\n");
            return 0;
        } else if (bytesRecv == 0){  // may not need this?
            printf("Client disconnected. Exiting...\n");
            return 0;
        }

        // at this point we have their message in buf and the size in bytesRecv
        if (strcmp(buf, "close") != 0){
            reply = processMsg(buf, cwd, REPLY_MAX_SIZE);
            char * sendString = (char*)calloc(strlen(reply)+1, sizeof(char));
            strcpy(sendString, reply);

            // send message back to the client
            send(sock, sendString, strlen(reply)+1, 0);
            free(sendString);
            free(reply);
        }
        else{  // close was sent
            printf("Connection was closed by client.");
            free(cwd);
            return 0;
        }  
    } 
}

/*
Function for proccessing the input sent from the client
Places the necessary response in char *reply, and returns a pointer to the reply in memory
*/
char* processMsg(char * msg, char * cwd, int REPLY_MAX_SIZE){
    // check if a shell (cmd prompt) command is being issued
    // an input that starts with "cmd" or "c"
    if (strcmp(strtok(msg, " "), "cmd") == 0 || strcmp(strtok(msg, " "), "c") == 0){
        msg+=(strcspn(msg, " ")+1); // pass everthing after the "cmd " or "c " at the beginning of the msg
        return processShellCmd(msg, cwd, REPLY_MAX_SIZE);
    }
    else {  // it is a command not meant to be executed by the shell
        // LATER: THIS MIGHT NOT WORK BECAUSE STRTOK MANGLES THE STRING THAT YOU PUT INTO IT
        char * reply = (char*)calloc(REPLY_MAX_SIZE, sizeof(char));
        strcpy(reply, "That is not a shell command and is currently not supported\n");
        return reply;
    }
}

/*
Process and edit command before sending it to the system to obtain output.
Returns the output from the command
*/
char* processShellCmd(char * cmd, char * cwd, int REPLY_MAX_SIZE) {
    // everything related to current working directory
    // if it is a cd command, change the current working directory, if it's not, prepend the cd to cwd and then getCmdOut
    char cmdCopy[64];
    strcpy(cmdCopy, cmd);
    char * arg;
    if (strcmp(strtok(cmdCopy, " "), "cd") == 0 && (arg=strtok(NULL, " ")) != NULL){
        if (strcmp(arg, "..") == 0){ // then cd to parent directory
            for (int i = strlen(cwd)-1; i >= 0; i--){
                if (*(cwd+i) == '\\'){
                    *(cwd+i) = '\0';
                    break;
                }
            }
        }
        else{
            // just cd to a normal dir, not the parent (..)
            // currently this only supports full paths
            strcpy(cwd, arg);

            // perhaps if the second char in arg is NOT a : (shows its not a full path [C:\dir\dir..])
            // then we could append it to the cwd and call that the new cwd??

            // Additionally TODO : make it so you can run the command C: or D: to change drives!
        }
        return strdup(cwd);
    }
    else{  // it is not a cd command with args (could be cd with no args)
        char finalCmd[512];
        char cwdDrive[8] = "";

        strncpy(cwdDrive, cwd, 2);
        sprintf(finalCmd, "%s && cd %s && %s 2>&1", cwdDrive, cwd, cmd);

        //printf("FINAL COMMAND: %s\n", finalCmd);
        return getCmdOut(finalCmd, REPLY_MAX_SIZE);
    }
}

/*
Returns output from the given command. Size is limited to REPLY_MAX_SIZE
*/
char* getCmdOut(char * cmd, int REPLY_MAX_SIZE){
    char * output = (char*)calloc(REPLY_MAX_SIZE, sizeof(char));
    char line[REPLY_MAX_SIZE];
    FILE *fp;

    fp = popen(cmd, "r");
    int bytesCat = 0;   // keeps track of the # of bytes concatenated to prevent buffer overflow 
    while (fgets(line, REPLY_MAX_SIZE, fp) != NULL) {
        bytesCat+=(strlen(line));
        strncat(output, line, REPLY_MAX_SIZE-bytesCat);
        if (bytesCat >= REPLY_MAX_SIZE){
            break;
        }
    }
    return output;
}