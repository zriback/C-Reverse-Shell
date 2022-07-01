#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include "interface.h"
#include "shell.h"

int main() {
    struct Shell shell;
    int ip[] = {192,168,1,1};
    shell = createShell(1, 4444, ip);

    printf("%d\n", shell.type);
    printf("%d\n", shell.port);
    
    for (int i = 0; i < 4; i++) {
        printf("%d", shell.ip[i]);
        if (i != 3){
            printf(".");
        }
    }
    printf("\n");

}