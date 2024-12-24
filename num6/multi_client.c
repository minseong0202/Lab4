#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1024

int sock;


void *receive_message(void *arg) {
    char buffer[BUFFER_SIZE];
    int read_size;

    while (1) {
 
        read_size = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (read_size > 0) {
            buffer[read_size] = '\0';  
            printf("\n[New Message] %s\n", buffer); 
            printf("Enter message: ");
        } else {
            printf("Disconnected from server\n");
            break;
        }
    }
    return NULL;
}

int main() {
    struct sockaddr_in server_addr;
    char message[BUFFER_SIZE];


    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server\n");


    pthread_t receive_thread;
    if (pthread_create(&receive_thread, NULL, receive_message, NULL) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }


    while (1) {
        printf("Enter message: ");
        fgets(message, BUFFER_SIZE, stdin);
        send(sock, message, strlen(message), 0);
    }

    close(sock);
    return 0;
}
