#ifndef SHELL_HEADER
#define SHELL_HEADER

struct Shell createShell(int type, int port, int ip[4]);

// Holds basic members pertaining to the shell we are defining
struct Shell {
    // 0=bind shell, 1=reverse shell
    int type;

    // port to connect on
    int port;

    // attacker's ip address (may not be needed for bind shell?)
    int * ip; 
};

#endif