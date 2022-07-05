#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Two macros for stringizing s
// More info at https://gcc.gnu.org/onlinedocs/cpp/Argument-Prescan.html#Argument-Prescan
#define xstr(s) str(s)
#define str(s) #s

int port;
int ip[4];




int main(){
    const char * ip_str;
    #ifdef PORT
        port = PORT;
    #endif

    #ifdef IP
        ip_str = xstr(IP);
    #endif
    
    printf("Port: %d\n", port);
    printf("IP: %s\n", ip_str);

    return 0;
}