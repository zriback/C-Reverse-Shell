#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

struct Shell createShell(int type, int port, int ip[4]) {
    struct Shell shell;
    shell.type = type;
    shell.port = port;
    
    shell.ip = (int*)malloc(4*sizeof(int));
    shell.ip = ip;
    
    return shell;
}



