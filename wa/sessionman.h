// include

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h> // memset()
#include <unistd.h> // close()

#include <time.h>

// definition

#define PORT (in_port_t)50002
#define MAX_ATTENDANTS 100
#define BUF_LEN 300

// type

typedef struct
{
    int fd;
    char name[16];
} ATTENDANT;

// variable

/*static */fd_set mask;
/*static */int width;
/*static */int attendants;
int num;
int soc;

int fd_self;
int fd_interrupt;

ATTENDANT p[MAX_ATTENDANTS];
char buf[BUF_LEN + 1];

// prototype declaration

void enter(int i, int fd);
void sessionman_init(int num, int maxfd);
void sessionman_loop();
void *live_accept(void *arg);
void send_all(int i, int n);
void ending();
void send_one(ATTENDANT sender, ATTENDANT recver, const char *info, int info_length, const char *sep, int sep_length, const char *content, int content_length);
void end_user(int i);
void generate_userlist(char *buffer, int buf_size, const ATTENDANT member[], int member_size);
void send_all_with_ret(int i, int n);
void write_leave(ATTENDANT leave_user);
