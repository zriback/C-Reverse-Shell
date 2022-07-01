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
    int type = -1;
    int port = -1;
    int ip[4];

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

            printf("%s %s\n", param, val);

            // if else ladder
            if (strcmp(param, "type") == 0) {
                type = atoi(val);
            }
            else if (strcmp(param, "port") == 0) {
                port = atoi(val);
            }
            else if (strcmp(param, "ip") == 0) {
                
                char * octet;
                octet = strtok(val, ".");
                ip[0] = atoi(octet);
                for (int i = 1; i < 4; i++) {
                    octet = strtok(NULL, ".");
                    ip[i] = atoi(octet);
                }   
            }
            else {
                printf("%s is not recognized.\nTry using type=n, port=n, or ip=x.x.x.x", param);
            }
        }
        else { // there is no = in the input
            if (strcmp(input, "CREATE") == 0){

                shell = createShell(type, port, ip);
                
                printf("type=%d, port=%d\n", shell.type, shell.port);
                // printf("%f", sizeof(shell.ip)/sizeof(int));
                for (int i = 0; i < 4; i++){
                    printf("%d ", shell.ip[i]);
                }
            }
        }
        
        

        // USE THE CREATE SHELL VARIABLE AND THEN USE LOCAL VARIABLES HERE INSTEAD OF WHAT WE ARE DOING RIGHT NOW
        
    }
}