#include "interface.h"
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include "shell.h"

// start the text interface so the user can configure their shell
void startInterface() {
    printf("--- STARTING INTERFACE ---\n");

    struct Shell shell;

    // creating our shell with default values
    shell.type = 0;
    shell.port = 4444;
    shell.ip = "192.168.1.1";

    char input[64];
    char * param = (char*)calloc(32, sizeof(char*));
    char * val = (char*)calloc(32, sizeof(char*));
    const char equals = '=';

    while (1) {
        printf("> ");
        
        scanf("%s", input);

        if (strchr(input, equals) != NULL){ // there is an = in the input
            strcpy(param, strtok(input, "="));
            strcpy(val, strtok(NULL, "="));

            printf("%s=%s\n", param, val);

            // if else ladder
            if (strcmp(param, "type") == 0) {
                shell.type = atoi(val);
            }
            else if (strcmp(param, "port") == 0) {
                shell.port = atoi(val);
            }
            else if (strcmp(param, "ip") == 0) {
                shell.ip = val;
            }
            else {
                printf("%s is not recognized.\nTry using type=n, port=n, or ip=x.x.x.x\n", param);
            }
        }
        else { // there is no = in the input
            if (strcmp(input, "create") == 0){
                printf("Creating shell with the following settings\n");
                printf("type=%d, port=%d\nip=%s\n", shell.type, shell.port, shell.ip);
                createShellExecutable(shell);
            }
            else if (strcmp(input, "exit") == 0) {
                printf("\nExiting...\n\n");
                break;
            }
            else if (strcmp(input, "help") == 0) {
                printf("Use type=x, port=x, and ip=x.x.x.x to set settings for the shell.\n");
                printf("create - creates shell with the current settings.\n");
                printf("exit - exit the interface.\n");
                printf("help - bring up this help menu.\n");
            } else {
                printf("Use \'help\' to bring up the help menu.\n");
            }
        }
          
    }
    free(param);
    free(val);

}