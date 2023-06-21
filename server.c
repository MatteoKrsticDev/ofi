#include <stdio.h>
#include <time.h>

#ifdef _WIN32
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib")
    #define OS_NAME "Windows"
#elif __linux__
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define OS_NAME "Linux"
#elif __APPLE__
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define OS_NAME "macOS"
#else
    #error "Unsupported operating system"
#endif

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
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    char request_buffer[REQUEST_BUFFER_SIZE];

#ifdef _WIN32
    WSADATA wsa;
    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock\n");
        return 1;
    }
#endif

    // Create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Failed to create socket\n");
        return 1;
    }

    // Prepare server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(DEFAULT_PORT);

    // Bind server socket to the specified address and port
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Failed to bind socket\n");
#ifdef _WIN32
        closesocket(server_socket);
#else
        close(server_socket);
#endif
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Failed to listen on socket\n");
#ifdef _WIN32
        closesocket(server_socket);
#else
        close(server_socket);
#endif
        return 1;
    }

    printf("HTTP Server running on %s\n", OS_NAME);
    printf("Listening on port %d\n", DEFAULT_PORT);

    while (1) {
        int addr_len = sizeof(client_addr);

        // Accept a new connection
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, (socklen_t*)&addr_len);
        if (client_socket == INVALID_SOCKET) {
            printf("Failed to accept connection\n");
#ifdef _WIN32
            closesocket(server_socket);
#else
            close(server_socket);
#endif
            return 1;
        }

        // Receive HTTP request
        memset(request_buffer, 0, sizeof(request_buffer));
        int recv_result = recv(client_socket, request_buffer, REQUEST_BUFFER_SIZE - 1, 0);
        if (recv_result == SOCKET_ERROR) {
            printf("Failed to receive data\n");
#ifdef _WIN32
            closesocket(client_socket);
#else
            close(client_socket);
#endif
            continue;
        }

        printf("Received request:\n%s\n", request_buffer);

        // Construct the HTTP response
        const char* response_header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
        int response_header_length = strlen(response_header);
        int html_content_length = strlen(html_content);

        // Send HTTP response header
        int send_result_header = send(client_socket, response_header, response_header_length, 0);
        if (send_result_header == SOCKET_ERROR) {
            printf("Failed to send response header\n");
#ifdef _WIN32
            closesocket(client_socket);
#else
            close(client_socket);
#endif
            continue;
        }

        // Send HTML content
        int send_result_content = send(client_socket, html_content, html_content_length, 0);
        if (send_result_content == SOCKET_ERROR) {
            printf("Failed to send response content\n");
#ifdef _WIN32
            closesocket(client_socket);
#else
            close(client_socket);
#endif
            continue;
        }

        // Close client socket
#ifdef _WIN32
        closesocket(client_socket);
#else
        close(client_socket);
#endif
    }

#ifdef _WIN32
    closesocket(server_socket);
    WSACleanup();
#else
    close(server_socket);
#endif

    return 0;
}
