#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT (in_port_t)50002
#define HOSTNAME_LENGTH 64

extern int session_soc;
extern char client_name[16];

extern void session_init(int soc);
extern void session_loop();
extern void die();

#define BUF_LEN 300

#define SEND_WIN_WIDTH 60
#define SEND_WIN_HEIGHT 3

#define RECV_WIN_WIDTH 60
#define RECV_WIN_HEIGHT 13
