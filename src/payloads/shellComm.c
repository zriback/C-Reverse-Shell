#include "shellComm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fileapi.h>

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>

// Need to link with Ws2_32.lib
// unfortunately this does not work, so when compiling we manually tell gcc to use this library to compile with the "-lws2_32" op tion.
#pragma comment (lib, "ws2_32.lib")


/*
Communicates with the given socket from the server/shell side
Return values:
    0 - the connection should be ended
    1 - continue the connection
*/
int comm(SOCKET sock, int BUF_SIZE, int REPLY_MAX_SIZE){
    char buf[BUF_SIZE];     // space for reply from client
    FILE *reply;            // space for message to client

    // default working directory
    char * cwd;
    cwd = getCmdOutStr("echo %HOMEDRIVE%%HOMEPATH%", 128);
    cwd[strcspn(cwd, "\n")] = '\0';

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
            reply = processMsg(buf, cwd);

            int allDataSent = 0;

            while(!allDataSent){

                char * sendString = (char*)calloc(REPLY_MAX_SIZE, sizeof(char));
                char line[REPLY_MAX_SIZE];

                // Populate sendString with data from the tmp file returned from processMsg()
                void * readResult;
                int bytesCat = 0;
                while((readResult = fgets(line, REPLY_MAX_SIZE - bytesCat - 1, reply)) != NULL){
                    strncat(sendString, line, REPLY_MAX_SIZE-bytesCat-1);
                    bytesCat+=(strlen(line));
                    if (bytesCat >= REPLY_MAX_SIZE-2){ // -2 because cannot concatenate a string of length 1
                        break;
                    }
                }
                if (readResult == NULL){ // then EOF was reached and all data has been sent
                    allDataSent = 1;
                }

                // if this is the final block of data to be sent, add the ETX byte
                if (allDataSent){
                    *(sendString+strlen(sendString)) = 0x03;
                }

                // send message back to the client
                send(sock, sendString, strlen(sendString), 0);
                
                free(sendString);            
            }
            fclose(reply);

            // remove the reply file if it is temp file in memory
            char path[64];
            char filename[260];
            GetTempPathA(sizeof(path), path);
            GetTempFileNameA(path, "buf", 1, filename);

            if (GetFileAttributesA(filename) != INVALID_FILE_ATTRIBUTES){ // the file exists, so remove it
                remove(filename);
            }

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
FILE* processMsg(char * msg, char * cwd){
    // check if a shell (cmd prompt) command is being issued
    // an input that starts with "cmd" or "c"
    char msgCpy[128];
    strncpy(msgCpy, msg, sizeof(msgCpy));
    if (strcmp(strtok(msgCpy, " "), "cmd") == 0 || strcmp(strtok(msgCpy, " "), "c") == 0){
        msg+=(strcspn(msg, " ")+1); // pass everthing after the "cmd " or "c " at the beginning of the msg
        return processShellCmd(msg, cwd);
    }
    else {  // it is a command not meant to be executed by the shell
        return processExtCmd(msg, cwd);
    }
}

/*
Process external commands. Perform function and return output
*/
FILE* processExtCmd(char * cmd, char * cwd){
    char cmdCpy[128];
    strncpy(cmdCpy, cmd, sizeof(cmd));

    if (strcmp(cmd, "reset") == 0){
        char * temp;
        temp = getCmdOutStr("echo %HOMEDRIVE%%HOMEPATH%", 128);
        temp[strcspn(temp, "\n")] = '\0';
        strcpy(cwd, temp);
        free(temp);

        return getTempFile(cwd);
    }
    else if (strcmp(strtok(cmdCpy, " "), "transfer") == 0) {
        char * filename = cmd;
        filename += (strcspn(cmd, " ")+1); // get the entire second argument whether or not it includes spaces
        return getTempFile("The transfer command has not been implemented yet.");
    }
    else{
        return getTempFile("That command is not recognized!\n");
    }
}

/*
Process and edit command before sending it to the system to obtain output.
Returns the output from the command
*/
FILE* processShellCmd(char * cmd, char * cwd) {
    // everything related to current working directory
    // if it is a cd command, change the current working directory, if it's not, prepend the cd to cwd and then getCmdOut
    char cmdCopy[64];
    strncpy(cmdCopy, cmd, sizeof(cmdCopy));
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
        else if (*(arg+1) == ':') { // the second char is a : then it is a full path
            strcpy(cwd, arg);
        }
        else { // then it is a relative path
            char temp[128];
            char * dirptr = cmd;
            dirptr += (strcspn(cmd, " ")+1);
            
            sprintf(temp, "%s\\%s", cwd, dirptr);
            strcpy(cwd, temp);
        }
        return getTempFile(cwd);
    }
    else if (*(cmd+1) == ':' && arg == NULL && strlen(cmd) == 2) {  // it's change drive command (D: for example)
        strcpy(cwd, cmd);
        return getTempFile(cwd);
    }
    else {  // it is a normal command
        char finalCmd[512];
        char cwdDrive[8] = "";

        strncpy(cwdDrive, cwd, 2);
        sprintf(finalCmd, "%s 2>&1 && cd %s\\ 2>&1 && %s 2>&1", cwdDrive, cwd, cmd);

        // printf("FINAL COMMAND: %s\n", finalCmd);
        return getCmdOut(finalCmd);
    }
}

/*
Returns output from the given command. Size is limited to 
*/
FILE* getCmdOut(char * cmd){
    FILE *fp;
    fp = popen(cmd, "r");
    return fp;
}

/*
Returns output from the given command. Size is limited to REPLY_MAX_SIZE
*/
char* getCmdOutStr(char * cmd, int REPLY_MAX_SIZE){
    char * output = (char*)calloc(REPLY_MAX_SIZE, sizeof(char));
    char line[REPLY_MAX_SIZE];
    FILE *fp;

    fp = popen(cmd, "r");
    int bytesCat = 0;   // keeps track of the # of bytes concatenated to prevent buffer overflow 
    while (fgets(line, REPLY_MAX_SIZE, fp) != NULL) {
        strncat(output, line, REPLY_MAX_SIZE-bytesCat-1);
        bytesCat+=(strlen(line));
        if (bytesCat >= REPLY_MAX_SIZE-1){
            break;
        }
    }
    fclose(fp);
    return output;
}

/*
Returns a pointer to a temp file created with the given text written into it
The name/path of the file is placed into the filename parameter
The name/path of the file is preserved solely so it can be deleted later
*/
FILE* getTempFile(char * string){
   FILE* file;
   char path[64];
   char filename[260];
   
   GetTempPathA(sizeof(path), path);
   int result = GetTempFileNameA(path, "buf", 1, filename);
   if (result == 0){
      return NULL;
   }
   file = fopen(filename, "w+b");

   fprintf(file, string);

   // seek back to the beginning of the file
   fseek(file, 0, SEEK_SET);

   return file;
}