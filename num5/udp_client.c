/* udp_client.c */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 5000          /* 서버 포트 */
#define MAXDATASIZE 100    /* 최대 데이터 크기 */

int main(int argc, char *argv[]) {
    int csock;                      // 클라이언트 소켓
    char buf[MAXDATASIZE];          // 송수신 버퍼
    struct sockaddr_in to_addr;     // 서버 주소 구조체
    struct sockaddr_in from_addr;   // 클라이언트 주소 구조체
    int from_len;                   // 클라이언트 주소 길이
    int len;                         // 메시지 길이
    int recv_len;                   // 수신된 메시지 길이

    // 인자가 정확하지 않으면 오류 메시지 출력
    if (argc != 3) {
        fprintf(stderr, "usage: udp_client <SERVER IP> <ECHOSTRING>\n");
        exit(1);
    }

    /* 클라이언트 소켓 생성 */
    if ((csock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    /* 서버 주소 설정 */
    memset(&to_addr, 0, sizeof(to_addr));  // 구조체 초기화
    to_addr.sin_family = AF_INET;           // 주소 체계 (IPv4)
    to_addr.sin_addr.s_addr = inet_addr(argv[1]);  // 서버 IP 주소
    to_addr.sin_port = htons(PORT);         // 서버 포트 번호

    /* 메시지 송신 */
    memset(buf, 0, MAXDATASIZE);           // 버퍼 초기화
    strcpy(buf, argv[2]);                  // 명령행 인수로 받은 메시지 저장
    len = strlen(buf);                     // 메시지 길이 계산

    // 서버로 메시지 전송
    if (sendto(csock, buf, len, 0, (struct sockaddr *)&to_addr, sizeof(to_addr)) != len) {
        fprintf(stderr, "send failed...\n");
        exit(1);
    }

    /* 메시지 수신 */
    memset(buf, 0, MAXDATASIZE);           // 버퍼 초기화
    from_len = sizeof(from_addr);          // 클라이언트 주소 길이 설정
    // 서버로부터 응답 수신
    if ((recv_len = recvfrom(csock, buf, MAXDATASIZE, 0, (struct sockaddr *)&from_addr, &from_len)) == -1) {
        perror("recv");
        exit(1);
    }

    // 받은 메시지 출력
    printf("Received: %s\n", buf);

    /* 소켓 종료 */
    close(csock);  // 소켓 닫기

    return 0;
}
