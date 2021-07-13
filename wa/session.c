#include <sys/types.h>
#include <signal.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "commands.h"
#include <sys/ioctl.h>
#include "session.h"
// #include <locale.h>

static WINDOW *win_send, *win_recv;

static char send_buf[BUF_LEN];
static char recv_buf[BUF_LEN];

int session_soc;
static fd_set mask;
static int width;

struct winsize ws = {.ws_row = 20, .ws_col = 60};

char client_name[16];         // 16
char *sep = "@send-buffer# "; // win_send の区切り文字

void die();
void write_header(WINDOW *win);

void session_init(int soc)
{
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);

    // setlocale( LC_ALL, "" );

    session_soc = soc;
    width = soc + 1;
    FD_ZERO(&mask);
    FD_SET(0, &mask);
    FD_SET(soc, &mask);

    initscr();
    signal(SIGINT, die);

    win_send = newwin(SEND_WIN_HEIGHT, ws.ws_col, 1, 0);

    scrollok(win_send, TRUE);
    wmove(win_send, 0, 0);

    // +1 はメッセージの最後の改行によって作られた空の行を隠すため(win_sendの後ろに隠す)
    win_recv = newwin(ws.ws_row - SEND_WIN_HEIGHT + 1, ws.ws_col, 0, 0);

    scrollok(win_recv, TRUE);
    wmove(win_recv, 0, 0);

    cbreak();
    noecho();

    wrefresh(win_recv);
    wrefresh(win_send);
}

void session_loop()
{
    int c;
    int flag = 1;
    fd_set read0k;
    int len = 0;
    int i;
    int y, x;
    int n;

    write_header(win_send);

    while (1)
    {
        read0k = mask;
        select(width, (fd_set *)&read0k, NULL, NULL, NULL);

        if (FD_ISSET(0, &read0k))
        {
            c = getchar();

            if (c == '\b' || c == 0x10 || c == 0x7F)
            {
                if (len == 0)
                {
                    continue;
                }

                len--;

                getyx(win_send, y, x);
                wmove(win_send, y, x - 1);
                waddch(win_send, ' ');
                wmove(win_send, y, x - 1);
            }
            else if (c == '\n' || c == '\r')
            {
                send_buf[len++] = '\0';

                if (!strcmp(send_buf, CMD_EXIT)) // strstr -> strcmp
                {
                    // exit flag
                    flag = 0;
                }

                write(session_soc, send_buf, len);

                wclear(win_send);
                len = 0;

                write_header(win_send);

                wrefresh(win_recv);
                wrefresh(win_send);
            }
            else
            {
                send_buf[len++] = c;
                waddch(win_send, c);
            }

            wrefresh(win_send);
        }

        if (FD_ISSET(session_soc, &read0k))
        {
            n = read(session_soc, recv_buf, BUF_LEN);

            for (i = 0; i < n; i++)
            {
                waddch(win_recv, recv_buf[i]);
            }

            getyx(win_recv, y, x);

            if (recv_buf[n - 1] == '\n')
            {
                // 送られてきた文字列の最後の文字が改行のとき
                // ＝メッセージの終端である
                // → カーソルのある行(＝空行)と同じ行にウィンドウを置く(空行を隠す)
                mvwin(win_send, y /* + 1*/, 0);
            }
            else
            {
                // 送られてきた文字列の最後の文字が改行じゃないとき
                // ＝メッセージの終端ではない
                // → カーソルの1つ下にウィンドウを置く
                mvwin(win_send, y + 1, 0);
            }

            // 動かす前の win_send の中身が残ってしまっているので win_recvをリフレッシュ
            wrefresh(win_recv);
            // win_recv が前面に来てしまうのでリフレッシュ
            wrefresh(win_send);
        }

        if (flag == 0)
        {
            break;
        }
    }

    die();
}

void die()
{
    endwin();
    close(session_soc);
    exit(0);
}

void write_header(WINDOW *win)
{
    int i, clen;

    // disp client name to win_send
    clen = strlen(client_name);
    for (i = 0; i < clen; i++)
    {
        waddch(win_send, client_name[i]);
    }

    // disp separator to win_send
    clen = strlen(sep);
    for (i = 0; i < clen; i++)
    {
        waddch(win_send, sep[i]);
    }
}