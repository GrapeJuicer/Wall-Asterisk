#include "mylib.h"

// 戻り値       ソケットのディスクリプタ
// port         接続先のポート番号
int setup_server(in_port_t port)
{
    // 変数宣言
    struct sockaddr_in me; // サーバー(自分)の情報
    int soc_waiting;       // listen するソケット
    int soc;               // 送受信に使うソケット

    // サーバー(自分)アドレスを sockaddr_in 構造体に格納
    memset((char *)&me, 0, sizeof(me));
    me.sin_family = AF_INET;
    me.sin_addr.s_addr = htonl(INADDR_ANY);
    me.sin_port = htons(port);

    // IPv4 でストリーム型のソケットを作成
    if ((soc_waiting = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        return -1;
    }

    // サーバー(自分)のアドレスをソケットに設定
    if (bind(soc_waiting, (struct sockaddr *)&me, sizeof(me)) == -1)
    {
        perror("bind");
        return -1;
    }

    // ソケットで待ち受けることを設定
    listen(soc_waiting, 1);

    fprintf(stderr, "successfully bound, now waiting.\n");

    // 接続要求が来るまでブロックする
    soc = accept(soc_waiting, NULL, NULL);

    // 接続待ちのためのソケットを閉じる
    close(soc_waiting);

    // 通信に使うソケットのディスクリプタを返す
    return soc;
}
