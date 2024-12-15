#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void DieWithError(char *errorMessage);
void helpme();
int check_ipaddress(const char *ip_address);

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: ./port_scan <IP> or ./port_scan help\n");
        return 1;
    }

    if (strcmp(argv[1], "help") == 0) {
        helpme();
    }

    // IPアドレスが形式的に正しいか確認
    const int mode = check_ipaddress(argv[1]);
    if (mode == 0) {
        printf("Invalid IP address format.\n");
        return 1;
    }

    // sockaddr 構造体の宣言と初期化
    struct sockaddr_in servAddr;
    struct sockaddr_in6 servAddr6;

    memset(&servAddr, 0, sizeof(servAddr));
    memset(&servAddr6, 0, sizeof(servAddr6));

    // アドレス設定
    if (mode == 1) {
        servAddr.sin_family = AF_INET;
        servAddr.sin_addr.s_addr = inet_addr(argv[1]);
    } else if (mode == 2) {
        servAddr6.sin6_family = AF_INET6;
        if (inet_pton(AF_INET6, argv[1], &servAddr6.sin6_addr) <= 0) {
            DieWithError("Invalid IPv6 address");
        }
    }

    // ポートスキャン
    for (unsigned int i = 0; i < 65535; i++) {
        int sock = 0;

        if (mode == 1) {
            // IPv4 ソケットの作成
            if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
                DieWithError("socket failed");
            }
            servAddr.sin_port = htons(i);

            // 接続試行
            if (connect(sock, (struct sockaddr *)&servAddr, sizeof(servAddr)) == 0) {
                printf("open/%u\n", i);
                close(sock);
            } else {
                close(sock);
            }
        } else if (mode == 2) {
            // IPv6 ソケットの作成
            if ((sock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP)) < 0) {
                DieWithError("socket6 failed");
            }
            servAddr6.sin6_port = htons(i);

            // 接続試行
            if (connect(sock, (struct sockaddr *)&servAddr6, sizeof(servAddr6)) == 0) {
                printf("open/%u\n", i);
                close(sock);
            } else {
                close(sock);
            }
        }
    }

    return 0;
}

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

void helpme()
{
    char text[] = "This program is a TCP port scanning system.\n"
                  "To use it, type ./port_scan <IP> to scan all ports.";
    printf("%s\n", text);
    exit(1);
}

int check_ipaddress(const char *ip_address)
{
    // IPv4なら1, IPv6なら2, 無効なら0を返す
    struct sockaddr_in sa;
    struct sockaddr_in6 sa6;
    if (inet_pton(AF_INET, ip_address, &(sa.sin_addr)) == 1) return 1;
    if (inet_pton(AF_INET6, ip_address, &(sa6.sin6_addr)) == 1) return 2;
    return 0;
}
