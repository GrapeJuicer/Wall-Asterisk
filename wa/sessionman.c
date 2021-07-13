#include "sessionman.h"
#include "commands.h"

void enter(int i, int fd)
{
    int len;

    // set file descriptor
    p[i].fd = fd;

    // initialize
    memset(p[i].name, 0, 16);

    // get/set username
    len = read(fd, p[i].name, 16);

    // adjust username
    p[i].name[len - 1] = '\0';
}

void sessionman_loop()
{
    fd_set read0k;
    int i;
    int at;

    while (1)
    {
        // stop process while attendants is 0
        while (!attendants);

        // get val
        at = attendants;
        read0k = mask;

        // monitor the socket
        select(width, (fd_set *)&read0k, NULL, NULL, NULL);

        // if a message is recieved from the system
        if (FD_ISSET(fd_interrupt, &read0k))
        {
            int n;

            // read system message
            n = read(fd_interrupt, buf, BUF_LEN);

            // log
            printf("received message from \'%s\': %s\n", "system", buf);

            // 送ってくるときは strlenを使って送ってくるので, '\0'が送られてこない
            buf[n++] = '\0';

            // send
            send_all_with_ret(-1, n); // -1はsystemからのメッセージ
            continue;
        }

        // if input is received from server's stdin
        if (FD_ISSET(0, &read0k))
        {
            // log
            printf("closing session ...\n");

            // close all connection
            ending();
        }

        for (i = 0; i < at; i++)
        {
            // if a message is recieved from the i-th user
            if (FD_ISSET(p[i].fd, &read0k))
            {
                int n;

                // read message
                n = read(p[i].fd, buf, BUF_LEN);

                if (n == 0) // strange message
                {
                    // get leaving user data
                    ATTENDANT lusr = p[i];

                    // log
                    printf("\033[1m\033[31msuspicious user was detected at \033[33m(fd: %d, name: %s)\033[31m. disconnect forcibly.\033[39m\033[0m\n", lusr.fd, lusr.name);

                    // diconnect leaving user
                    end_user(i);

                    // send leave message
                    write_leave(lusr);

                    // log
                    printf("\033[32mdisconnected\033[39m (fd: %d, name: %s, attendants: %d)\n", lusr.fd, lusr.name, attendants);

                    continue;
                }
                else if (n == 1) // empty message
                {
                    continue;
                }
                else // send message
                {
                    // log
                    printf("received message from \'%s\': %s\n", p[i].name, buf);
                    // n - 1 = strlen()

                    if (!strcmp(buf, CMD_EXIT)) // /exit
                    {
                        // log
                        printf("\033[36mcalled command:\033[39m %s: disconnect user\n", CMD_EXIT);

                        // get leaving (lv) user info
                        ATTENDANT eusr = p[i];

                        // disconnect connection with lv user
                        end_user(i); // attendants が 1 小さくなる

                        // send CMD_EXIT message
                        send_all_with_ret(i, n);

                        // send leave message
                        write_leave(eusr);

                        // log
                        printf("\033[32mdisconnected\033[39m (fd: %d, name: %s, attendants: %d)\n", eusr.fd, eusr.name, attendants);
                    }
                    else if (!strcmp(buf, CMD_USER)) // /user
                    {
                        // log
                        printf("\033[36mcalled command:\033[39m %s: create userlist\n", CMD_USER);

                        // send CMD_USER message
                        send_all_with_ret(i, n);

                        // generate user list
                        generate_userlist(buf, BUF_LEN, p, at);

                        // send userlist message
                        write(fd_self, buf, strlen(buf));
                    }
                    else
                    {
                        // send message
                        send_all_with_ret(i, n);
                    }
                }
            }
        }
    }
}

void send_all_with_ret(int i, int n)
{
    buf[n - 1] = '\n'; // clientから送られてくるデータにはすでに'\0'が入っている

    // send message
    send_all(i, n);
    buf[n - 1] = '\0'; // チェック用に '\0' に戻す
}

void write_leave(ATTENDANT leave_user)
{
    // if user exist
    if (attendants > 0)
    {
        // send leave message
        sprintf(buf, "User \'%s\' has left ...", leave_user.name);
        write(fd_self, buf, strlen(buf));
    }
}

void ending()
{
    int i;

    // generate message
    sprintf(buf, "Closing session. Press \'%s\' or Ctrl+C. Bye :)", CMD_EXIT);

    // send message
    send_all(-1, strlen(buf));
    // write(fd_self, buf, strlen(buf)); // closeしちゃうからwriteで送れない

    // close
    for (i = 0; i < attendants; i++)
    {
        close(p[i].fd);
    }

    exit(0);
}

void end_user(int i)
{
    // close connection
    close(p[i].fd);

    // delete fd from mask
    FD_CLR(p[i].fd, &mask);

    // alignment p[]
    p[i] = p[--attendants];
}

////////////////////////////////////////////////////////////////////////
// overview:
//      send_all is a function that sends a message to all members.
// arg:
//      int i : sender id. if you wanna set sender to 'system', set -1.
//      int n : buffer(char buf[BUF_LEN+1]) size.
// return:
//      none
////////////////////////////////////////////////////////////////////////
void send_all(int i, int n)
{
    int j;
    ATTENDANT sender;
    const char separator[] = "$ ";
    char date[64];
    time_t t_send;

    // get time
    t_send = time(NULL);
    strftime(date, sizeof(date), "@%H:%M", localtime(&t_send));

    if (i < 0) // system
    {
        // set sender to 'system'
        // sender.fd = fd_interrupt; // not necessary at the moment
        strcpy(sender.name, "system");
    }
    else // user
    {
        // set sender to i-th user
        sender = p[i];
    }

    // send message to all member
    for (j = 0; j < attendants; j++)
    {
        // send message to j-th user
        send_one(sender, p[j], date, strlen(date), separator, strlen(separator), buf, n);
    }
}

// send message to recver
void send_one(ATTENDANT sender, ATTENDANT recver, const char *info, int info_length, const char *sep, int sep_length, const char *content, int content_length)
{
    write(recver.fd, sender.name, strlen(sender.name)); // sender name
    write(recver.fd, info, info_length);                // date
    write(recver.fd, sep, sep_length);                  // separator
    write(recver.fd, content, content_length);          // content
}

// 最長になっても buf_size 番目に '\0' が来る
void generate_userlist(char *buffer, int buf_size, const ATTENDANT member[], int member_size)
{
    // show user list
    int i;
    const char *usr_etc = "etc ...";
    const char *usr_sep = ", ";
    const int usr_etc_len = strlen(usr_etc);
    const int usr_sep_len = strlen(usr_sep);

    // if user doesn't exist
    if (member_size < 1)
    {
        return;
    }

    // add user to buffer
    sprintf(buffer, "userlist: %s", p[0].name);

    for (i = 1; i < member_size; i++)
    {
        // check size
        if (strlen(buffer) + strlen(p[i].name) + usr_etc_len + usr_sep_len < buf_size)
        {
            // if there is free space in the buffer
            strcat(buffer, usr_sep);
            strcat(buffer, p[i].name);
        }
        else
        {
            // if there is no space in the buffer
            strcat(buffer, usr_sep);
            strcat(buffer, usr_etc);
        }
    }
}