#include <stdio.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#define DEFAULT_PORT 8080
#define REQUEST_BUFFER_SIZE 512
#define RESPONSE_BUFFER_SIZE 512

const char* html_content =
"<!DOCTYPE html>\r\n"
"<html lang=\"en\">\r\n"
"<head>\r\n"
"    <meta charset=\"UTF-8\">\r\n"
"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\r\n"
"    <title>ofi</title>\r\n"
"</head>\r\n"
"<body>\r\n"
"    <h1>this is c baby</h1>\r\n"
"</body>\r\n"
"</html>";

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    char request_buffer[REQUEST_BUFFER_SIZE];

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        printf("\033[0;31mFailed to create socket\n\033[0m");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(DEFAULT_PORT);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        printf("\033[0;31mFailed to bind socket\n\033[0m");
        close(server_socket);
        return 1;
    }

    if (listen(server_socket, SOMAXCONN) == -1) {
        printf("\033[0;31mFailed to listen on socket\n\033[0m");
        close(server_socket);
        return 1;
    }

    printf("\033[0;32mHTTP Server running on Linux\n\033[0m");
    printf("\033[0;36mListening on port %d\n\033[0m", DEFAULT_PORT);

    while (1) {
        int addr_len = sizeof(client_addr);

        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, (socklen_t*)&addr_len);
        if (client_socket == -1) {
            printf("\033[0;31mFailed to accept connection\n\033[0m");
            close(server_socket);
            return 1;
        }

        memset(request_buffer, 0, sizeof(request_buffer));
        int recv_result = recv(client_socket, request_buffer, REQUEST_BUFFER_SIZE - 1, 0);
        if (recv_result == -1) {
            printf("\033[0;31mFailed to receive data\n\033[0m");
            close(client_socket);
            continue;
        }

        printf("Received request:\n%s\n", request_buffer);

        const char* response_header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
        int response_header_length = strlen(response_header);
        int html_content_length = strlen(html_content);

        int send_result_header = send(client_socket, response_header, response_header_length, 0);
        if (send_result_header == -1) {
            printf("\033[0;31mFailed to send response header\n\033[0m");
            close(client_socket);
            continue;
        }

        int send_result_content = send(client_socket, html_content, html_content_length, 0);
        if (send_result_content == -1) {
            printf("\033[0;31mFailed to send response content\n\033[0m");
            close(client_socket);
            continue;
        }

        close(client_socket);
    }

    close(server_socket);

    return 0;
}
