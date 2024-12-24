#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 3490             
#define BUF_SIZE 128

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server_addr;
    char msg[BUF_SIZE];
    int str_len;


    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket() error");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        exit(1);
    }

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect() error");
        exit(1);
    }

    printf("Connected to server. Type messages (type 'exit' to quit):\n");


    while (1) {
        printf("Input message: ");
        fgets(msg, BUF_SIZE, stdin);

        if (!strcmp(msg, "exit\n")) {
            printf("Client exiting...\n");
            break;
        }

   
        write(sock, msg, strlen(msg));


        str_len = read(sock, msg, BUF_SIZE - 1);
        if (str_len == -1) {
            perror("read() error");
            exit(1);
        }

        msg[str_len] = '\0';
        printf("Server response: %s\n", msg);
    }

    // 4. 소켓 닫기
    close(sock);
    return 0;
}
