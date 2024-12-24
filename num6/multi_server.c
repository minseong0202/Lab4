#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int client_sockets[MAX_CLIENTS];

void broadcast_message(char *message, int sender_socket) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] != -1 && client_sockets[i] != sender_socket) {
            if (send(client_sockets[i], message, strlen(message), 0) < 0) {
                perror("send");
            }
        }
    }
}

void *handle_client(void *arg) {
    int client_socket = *((int *)arg);
    char buffer[BUFFER_SIZE];
    int read_size;

    while ((read_size = recv(client_socket, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[read_size] = '\0';  
        printf("Message from client %d: %s\n", client_socket, buffer);
        broadcast_message(buffer, client_socket); 
    }

    if (read_size == 0) {
        printf("Client %d disconnected\n", client_socket);
    } else if (read_size == -1) {
        perror("recv");
    }


    close(client_socket);


    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] == client_socket) {
            client_sockets[i] = -1;  
            break;
        }
    }

    return NULL; 
}

int main() {
    int server_socket, new_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);


    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_sockets[i] = -1;
    }


    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);


    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }


    if (listen(server_socket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        if ((new_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len)) < 0) {
            perror("accept");
            continue;
        }

        printf("New client connected: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

 
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] == -1) {
                client_sockets[i] = new_socket;
                break;
            }
        }

  
        pthread_t client_thread;
        if (pthread_create(&client_thread, NULL, handle_client, (void *)&new_socket) != 0) {
            perror("pthread_create");
            continue;
        }

      
    }

    close(server_socket);
    return 0;
}
