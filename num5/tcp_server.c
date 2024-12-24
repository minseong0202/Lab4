/* tcp_server.c */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <unistd.h> /* close() 함수 포함 */

#define MYPORT 3490      // 서버가 사용할 포트번호 : 3490
#define BACKLOG 10       // 서버가 대기할 수 있는 최대 연결 수 : 10
#define MAXBUF 100       // 버퍼 크기

int main() {
    int ssock, csock;
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    char buf[MAXBUF];
    socklen_t sin_size;

    /* 서버 소켓 생성 */
    if ((ssock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        // AF_INET : IPv4 주소 체계
        // SOCK_STREAM : TCP 프로토콜
        perror("socket");
        exit(1);
    }

    /* 서버 포트 설정 */
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(MYPORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY; // IP 주소를 INADDR_ANY로 설정하여 어떤 IP에서도 연결 가능 


    // 서버 소켓에 IP 주소와 포트 할당
    if (bind(ssock, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) == -1) { 
        perror("bind");
        exit(1);
    }

    /* 연결 요구 허락 */
    if (listen(ssock, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    while (1) { /* 클라이언트 요구 처리 */
        /* 클라이언트 소켓 정보 획득 */
        sin_size = sizeof(struct sockaddr_in);
        if ((csock = accept(ssock, (struct sockaddr *)&clnt_addr, &sin_size)) == -1) {
            perror("accept");
            continue;
        }

        printf("server: got connection from %s\n", inet_ntoa(clnt_addr.sin_addr));

        /* 메시지 송수신 */
        memset(buf, 0, MAXBUF);
        if (recv(csock, buf, MAXBUF, 0) == -1) {
            perror("recv");
            close(csock);
            continue;
        }

        if (send(csock, buf, strlen(buf), 0) == -1) {
            perror("send");
        }

        close(csock); /* 클라이언트 소켓 종료 */
    }

    close(ssock); /* 서버 소켓 종료 */
    return 0;
}
