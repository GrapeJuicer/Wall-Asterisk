#include <stdio.h>
#include <stdlib.h>
#include "sessionman.h"
#include "mylib.h"
#include <pthread.h>

#define SELF_PORT 50002

#define VERSION "1.0.0"

int main(int argc, char *argv[])
{
    pthread_t th;

    if (argc != 2)
    {
        fprintf(stderr, "Wall **** Server %s\n", VERSION);
        fprintf(stderr, "Usage: was <attendants>\n");
        fprintf(stderr, "   <attendants> is a max-number of members.\n");
        exit(1);
    }

    if ((num = atoi(argv[1])) <= 0 || num > MAX_ATTENDANTS)
    {
        fprintf(stderr, "attendants limit = %d\n", MAX_ATTENDANTS);
        exit(1);
    }

    if ((soc = mserver_socket(PORT, num)) == -1)
    {
        fprintf(stderr, "cannot setup server\n");
        exit(1);
    }

    FD_ZERO(&mask);   // mask の初期化
    FD_SET(0, &mask); // fd を mask に追加

    if (pthread_create(&th, NULL, live_accept, NULL /*&ldata*/) == -1)
    {
        fprintf(stderr, "failed to create thread\n");
        return -1;
    }

    // connection for interrupt
    if ((fd_self = setup_client("localhost", SELF_PORT)) == -1)
    {
        fprintf(stderr, "failed to setup client\n");
        return -1;
    }

    sessionman_loop();
}
