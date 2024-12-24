#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_client(int client_socket);
void execute_cgi(const char *script_path, const char *query_string, int client_socket);

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) == -1) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        if (client_socket == -1) {
            perror("Accept failed");
            continue;
        }

        handle_client(client_socket);
        close(client_socket);
    }

    close(server_socket);
    return 0;
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    char method[16], path[256], protocol[16];
    int bytes_read;

    bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0) {
        return;
    }

    buffer[bytes_read] = '\0';
    printf("Request:\n%s\n", buffer);

    sscanf(buffer, "%s %s %s", method, path, protocol);

    if (strcmp(method, "GET") == 0) {
        if (strstr(path, "/cgi-bin/") == path) {

            char *query_string = strchr(path, '?');
            if (query_string) {
                *query_string++ = '\0';
            }
            execute_cgi(path + 9, query_string, client_socket);
        } else {

            char full_path[BUFFER_SIZE] = ".";
            strcat(full_path, path);

            FILE *file = fopen(full_path, "r");
            if (file) {
                char response[BUFFER_SIZE];
                snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\n\r\n");
                write(client_socket, response, strlen(response));

                while (fgets(buffer, sizeof(buffer), file)) {
                    write(client_socket, buffer, strlen(buffer));
                }

                fclose(file);
            } else {
                const char *not_found = "HTTP/1.1 404 Not Found\r\n\r\nFile not found.";
                write(client_socket, not_found, strlen(not_found));
            }
        }
    } else if (strcmp(method, "POST") == 0) {
        const char *response = "HTTP/1.1 200 OK\r\n\r\nPOST request received.";
        write(client_socket, response, strlen(response));
    } else {
        const char *bad_request = "HTTP/1.1 400 Bad Request\r\n\r\nUnsupported method.";
        write(client_socket, bad_request, strlen(bad_request));
    }
}

void execute_cgi(const char *script_path, const char *query_string, int client_socket) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("Pipe failed");
        return;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("Fork failed");
        return;
    }

    if (pid == 0) {

        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        setenv("QUERY_STRING", query_string, 1);
        execl(script_path, script_path, NULL);
        perror("Exec failed");
        exit(EXIT_FAILURE);
    } else {

        close(pipefd[1]);
        char response[BUFFER_SIZE];
        snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\n\r\n");
        write(client_socket, response, strlen(response));

        char c;
        while (read(pipefd[0], &c, 1) > 0) {
            write(client_socket, &c, 1);
        }

        close(pipefd[0]);
        waitpid(pid, NULL, 0);
    }
}
