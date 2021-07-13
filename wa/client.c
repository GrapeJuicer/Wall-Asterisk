#include "session.h"
#include "mylib.h"
// #include <signal.h>
#include "commands.h"

#define VERSION "1.0.1"

// ./c ip username
int main(int argc, char *argv[])
{
    int soc;

    if (argc != 3)
    {
        fprintf(stderr, "Wall **** %s\n", VERSION);
        fprintf(stderr, "Usage: wa <IP-address> <username>\n");
        fprintf(stderr, "    <IP-address> is a IPv4 address. Ex: \'xxx.xxx.xxx.xxx\' or \'localhost\'.\n");
        fprintf(stderr, "    <username> is your handle-name. its length is less than 16 character (<16).\n");
        return -1;
    }

    if (strlen(argv[2]) >= 16)
    {
        fprintf(stderr, "Error: Username is too long\n");
        return -1;
    }
    
    
    if ((soc = setup_client(argv[1], PORT)) == -1) // ip
    {
        exit(1);
    }

    // username
    strncpy(client_name, argv[2], strlen(argv[2]));
    write(soc, argv[2], strlen(argv[2]) + 1);

    session_init(soc);

    session_loop();
}
