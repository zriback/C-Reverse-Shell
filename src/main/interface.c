#include "interface.h"
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include "shell.h"

// start the text interface so the user can configure their shell
void startInterface() {
    printf("--- STARTING INTERFACE ---\n");

    struct Shell shell;

    // creating our shell
    shell.type = 0;
    shell.port = 4444;
    shell.ip[0] = 192;
    shell.ip[1] = 168;
    shell.ip[2] = 1;
    shell.ip[3] = 1;

    char input[64];
    char * param;
    char * val;
    const char equals = '=';

    while (1) {
        printf("> ");
        
        scanf("%s", input);

        if (strchr(input, equals) != NULL){ // there is an = in the input
            param = strtok(input, "=");
            val = strtok(NULL, "=");

            printf("%s=%s\n", param, val);

            // if else ladder
            if (strcmp(param, "type") == 0) {
                shell.type = atoi(val);
            }
            else if (strcmp(param, "port") == 0) {
                shell.port = atoi(val);
            }
            else if (strcmp(param, "ip") == 0) {
                
                char * octet;
                octet = strtok(val, ".");
                shell.ip[0] = atoi(octet);
                for (int i = 1; i < 4; i++) {
                    octet = strtok(NULL, ".");
                    shell.ip[i] = atoi(octet);
                }   
            }
            else {
                printf("%s is not recognized.\nTry using type=n, port=n, or ip=x.x.x.x ", param);
            }
        }
        else { // there is no = in the input
            if (strcmp(input, "create") == 0){
                
                printf("Creating shell with the following settings\n");
                printf("type=%d, port=%d\nip=", shell.type, shell.port);
                
                for (int i = 0; i < 4; i++){
                    printf("%d", shell.ip[i]);
                    if (i != 3){
                        printf(".");
                    }
                }

            }
            else if ((input, "exit")) {
                printf("\nExiting...\n\n");
                break;
            }
            else if (strcmp(input, "help")) {
                printf("Use type=x, port=x, and ip=x.x.x.x to set settings for the shell.\n");
                printf("create - creates shell with the current settings.");
                printf("exit - exit the interface");
                printf("help - bring up this help menu.");
            } else {
                printf("Use \'help\' to bring up the help menu.");
            }
        }
        
        

        
        
    }
}