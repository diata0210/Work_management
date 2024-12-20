// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <arpa/inet.h>
// #include "socket.h"
// #include "config.h"  // Đảm bảo bạn có include file cấu hình này

// int connect_to_server(const char *server_ip, int port) {
//     int client_fd;
//     struct sockaddr_in server_addr;

//     // Tạo socket
//     client_fd = socket(AF_INET, SOCK_STREAM, 0);
//     if (client_fd == -1) {
//         perror("Error creating socket");
//         return -1;
//     }

//     // Cấu hình địa chỉ server
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_addr.s_addr = inet_addr(server_ip);
//     server_addr.sin_port = htons(port);

//     // Kết nối đến server
//     if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
//         perror("Error connecting to server");
//         close(client_fd);
//         return -1;
//     }

//     printf("Connected to server at %s:%d\n", server_ip, port);
//     return client_fd;
// }

// // Gửi thông điệp đến server
// int send_message(int client_fd, const char *message) {
//     int bytes_sent = send(client_fd, message, strlen(message), 0);
//     if (bytes_sent == -1) {
//         perror("Error sending message");
//         return -1;
//     }
//     return bytes_sent;
// }

// // Nhận thông điệp từ server
// int receive_message(int client_fd, char *buffer, int buffer_size) {
//     int bytes_received = recv(client_fd, buffer, buffer_size - 1, 0);
//     if (bytes_received == -1) {
//         perror("Error receiving message");
//         return -1;
//     }
//     buffer[bytes_received] = '\0';  // Kết thúc chuỗi
//     return bytes_received;
// }
#include "socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>


#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

static int sock_fd = -1;

bool initialize_socket() {
    
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sock_fd < 0) {
        perror("Socket creation failed");
        return false;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        close_socket();
        return false;
    }

    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close_socket();
        return false;
    }

    return true;
}

void close_socket() {
    if (sock_fd >= 0) {
        close(sock_fd);
        sock_fd = -1;
    }
}

bool send_request(const char *request, char *response) {
    if (sock_fd < 0) {
        if (!initialize_socket()) {
            return false;
        }
    }

    if (send(sock_fd, request, strlen(request), 0) < 0) {
        perror("Send failed");
        close_socket();
        return false;
    }

    if (response) {
        return receive_response(response, BUFFER_SIZE);
    }
    
    return true;
}

bool receive_response(char *response, int max_length) {
    int bytes_received = recv(sock_fd, response, max_length - 1, 0);
    if (bytes_received < 0) {
        perror("Receive failed");
        close_socket();
        return false;
    }
    
    response[bytes_received] = '\0';
    return true;
}
