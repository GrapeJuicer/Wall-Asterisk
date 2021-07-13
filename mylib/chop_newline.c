#include "mylib.h"

// 戻り値       与えられた文字列の先頭アドレス
// str          改行文字で終わっているかもしれない文字列
// len          処理の制限
char *chop_newline(char *str, int len)
{
    int n = strlen(str);

    // 末尾が改行文字なら削る
    if (n < len && str[n - 1] == '\n')
    {
        str[n - 1] = '\0';
    }

    // 先頭番地を返す
    return str;
}