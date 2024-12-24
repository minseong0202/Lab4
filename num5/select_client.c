/* select_client.c */
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

#define PORT 3490       
#define MAXDATASIZE 100    

int main(int argc, char *argv[]) {
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct sockaddr_in serv_addr;


    if (argc != 2) {
        fprintf(stderr, "usage: select_client <SERVER IP>\n");
        exit(1);
    }


    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }


    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    memset(&(serv_addr.sin_zero), 0, 8);


    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) == -1) {
        perror("connect");
        exit(1);
    }


    sprintf(buf, "Hello, server\n");
    if ((numbytes = send(sockfd, buf, strlen(buf), 0)) == -1) {
        perror("send");
        exit(1);
    }


    if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    buf[numbytes] = '\0'; 
    printf("Received: %s\n", buf);


    close(sockfd);
    return 0;
}
