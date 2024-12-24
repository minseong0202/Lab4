/* udp_server.c */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 5000           /* 서버 포트 */
#define BACKLOG 10          /* 대기 큐 크기 (UDP에서는 사용되지 않음) */
#define MAXBUF 100          /* 버퍼 크기 */

int main() {
    int sock;                        // 서버 소켓
    struct sockaddr_in serv_addr;    // 서버 주소 구조체
    struct sockaddr_in clnt_addr;    // 클라이언트 주소 구조체
    char buf[MAXBUF];                // 송수신 버퍼
    int clnt_addr_len;               // 클라이언트 주소 길이

    /* 서버 소켓 생성 */
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    /* 서버 포트 설정 */
    memset(&serv_addr, 0, sizeof(serv_addr));  // 구조체 초기화
    serv_addr.sin_family = AF_INET;            // 주소 체계 (IPv4)
    serv_addr.sin_port = htons(PORT);          // 서버 포트 번호
    serv_addr.sin_addr.s_addr = INADDR_ANY;    // 모든 IP 주소에서 접속 허용

    /* 소켓에 주소 바인딩 */
    if (bind(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("bind");
        exit(1);
    }

    while (1) {  // 클라이언트 요청 처리 반복
        /* 메시지 수신 */
        clnt_addr_len = sizeof(clnt_addr);  // 클라이언트 주소 길이 설정
        memset(buf, 0, MAXBUF);             // 버퍼 초기화
        if (recvfrom(sock, buf, MAXBUF, 0, (struct sockaddr *)&clnt_addr, &clnt_addr_len) == -1) {
            perror("recv");
            exit(1);
        }

        /* 클라이언트의 IP 주소 출력 */
        printf("server: got connection from %s\n", inet_ntoa(clnt_addr.sin_addr));

        /* 메시지 송신 */
        if (sendto(sock, buf, strlen(buf), 0, (struct sockaddr *)&clnt_addr, sizeof(clnt_addr)) == -1) {
            perror("send");
            close(sock);
            exit(1);
        }
    }

    /* 소켓 종료 */
    close(sock);
    return 0;
}
