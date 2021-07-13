#include "sessionman.h"
#include "mylib.h"

////////////////////////////////////////////////////////////////////////
// overview:
//        live_accept is a function that can always accept
//      socket connection by using multi-threading.
// arg:
//      void *arg : this argument doesn't use. so set NULL.
// return:
//      success: ?
//      error: NULL
////////////////////////////////////////////////////////////////////////
void *live_accept(void *arg)
{
    int fd;
    // static char *mesg = "Communication Ready.\n";
    char sysmsg_join[BUF_LEN];

    // set fd_interrupt to none
    fd_interrupt = -1;

    while (1)
    {
        // When the maximum number of participants is reached
        if (attendants >= num)
        {
            // pass accept
            continue;
        }

        // accept
        if ((fd = accept(soc, NULL, NULL)) == -1)
        {
            // error message
            fprintf(stderr, "accept error\n");

            // exit
            return NULL;
        }

        // set fd to mask
        FD_SET(fd, &mask);

        // set mask's width for select()
        if (fd + 1 > width)
        {
            width = fd + 1;
        }
        
        // if interrupt connection is not set
        if (fd_interrupt == -1)
        {
            // set fd
            fd_interrupt = fd;

            // accept connection for interrupt
            printf("accept interrupt connection. fd: %d\n", fd);
        }
        else
        {
            // call 'enter'
            enter(attendants, fd);

            // send ready message
            // write(fd, mesg, strlen(mesg));

            // create system messsage
            sprintf(sysmsg_join, "User \'%s\' has joined !", p[attendants].name);

            // increment attendants
            attendants++;

            // interrupt
            write(fd_self, sysmsg_join, strlen(sysmsg_join));

            // accept message
            printf("\033[32maccepted\033[39m (fd: %d, name: %s, attendants: %d)\n", fd, p[attendants - 1].name, attendants);
        }
    }
}
