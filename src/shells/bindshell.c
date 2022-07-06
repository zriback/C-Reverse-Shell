#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Two macros for stringizing s
// More info at https://gcc.gnu.org/onlinedocs/cpp/Argument-Prescan.html#Argument-Prescan
#define xstr(s) str(s)
#define str(s) #s

int port;
char * ip;




int main(){
    port = 4444;
    ip = (char*)calloc(16, sizeof(char));
    strcpy(ip, "192.168.1.1");

    #ifdef PORT
        port = PORT;
    #endif

    #ifdef IP
        ip = xstr(IP);
    #endif
    
    printf("Port: %d\n", port);
    printf("IP: %s\n", ip);

    free(ip);

    return 0;
}