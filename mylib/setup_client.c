#include "mylib.h"

// 戻り値       ソケットのディスクリプタ
// hostname     接続先のホスト名
// port         接続先のポート番号
int setup_client(char *hostname, in_port_t port)
{
    //サーバ(相手)の情報
    struct hostent *server_ent;
    //サーバ(相手)のアドレス
    struct sockaddr_in server;
    //ソケットのディスクリプト
    int soc;

    if ((server_ent = gethostbyname(hostname)) == NULL)
    {
        perror("gethostbyname");
        return -1;
    }

    memset((char *)&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    memcpy((char *)&server.sin_addr, server_ent->h_addr, server_ent->h_length);

    if ((soc = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        return -1;
    }

    // 接続
    if (connect(soc, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        perror("connect");
        return -1;
    }

    return soc;
}
