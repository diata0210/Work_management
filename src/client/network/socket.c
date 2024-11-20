#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "socket.h"
#include "config.h"  // Đảm bảo bạn có include file cấu hình này

int connect_to_server(const char *server_ip, int port) {
    int client_fd;
    struct sockaddr_in server_addr;

    // Tạo socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("Error creating socket");
        return -1;
    }

    // Cấu hình địa chỉ server
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(port);

    // Kết nối đến server
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error connecting to server");
        close(client_fd);
        return -1;
    }

    printf("Connected to server at %s:%d\n", server_ip, port);
    return client_fd;
}

// Gửi thông điệp đến server
int send_message(int client_fd, const char *message) {
    int bytes_sent = send(client_fd, message, strlen(message), 0);
    if (bytes_sent == -1) {
        perror("Error sending message");
        return -1;
    }
    return bytes_sent;
}

// Nhận thông điệp từ server
int receive_message(int client_fd, char *buffer, int buffer_size) {
    int bytes_received = recv(client_fd, buffer, buffer_size - 1, 0);
    if (bytes_received == -1) {
        perror("Error receiving message");
        return -1;
    }
    buffer[bytes_received] = '\0';  // Kết thúc chuỗi
    return bytes_received;
}
