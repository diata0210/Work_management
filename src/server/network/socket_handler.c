#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "socket_handler.h"
#include "control_message_handler.h"
#include "data_message_handler.h"
#include "chat_message_handler.h"
#include "video_message_handler.h"
#include "config.h"
#include "logger.h"

#define BUFFER_SIZE MAX_BUFFER

// Khởi tạo server socket
int setup_server_socket(int port) {
    int server_fd;
    struct sockaddr_in server_addr;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        log_error("Error creating socket");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        log_error("Error binding socket");
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, MAX_CONNECTIONS) == -1) {
        log_error("Error listening on socket");
        close(server_fd);
        return -1;
    }

    log_info("Server is listening on port %d", port);
    return server_fd;
}

// Chấp nhận kết nối từ client
int accept_client_connection(int server_fd) {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len);
    if (client_fd == -1) {
        log_error("Error accepting client connection");
        return -1;
    }

    log_info("Client connected with IP: %s and Port: %d",
             inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    return client_fd;
}

// Phân loại và xử lý các thông điệp từ client
void handle_client_message(int client_fd, const char* message) {
    if (strncmp(message, "CONTROL", 7) == 0) {
        handle_control_message(client_fd, message + 8);  // Gọi đến handler điều khiển
    } else if (strncmp(message, "DATA", 4) == 0) {
        handle_data_message(client_fd, message + 5);  // Gọi đến handler dữ liệu
    } else if (strncmp(message, "CHAT", 4) == 0) {
        handle_chat_message(client_fd, message + 5);  // Gọi đến handler chat
    } else if (strncmp(message, "VIDEO", 5) == 0) {
        handle_video_message(client_fd, message + 6);  // Gọi đến handler video
    } else {
        log_error("Unknown message type: %s", message);
    }
}

// Gửi dữ liệu tới client
int send_data(int client_fd, const char* data) {
    int bytes_sent = send(client_fd, data, strlen(data), 0);
    if (bytes_sent == -1) {
        log_error("Error sending data to client");
    }
    return bytes_sent;
}

// Nhận dữ liệu từ client
int receive_data(int client_fd, char* buffer, int buffer_size) {
    int bytes_received = recv(client_fd, buffer, buffer_size - 1, 0);
    if (bytes_received == -1) {
        log_error("Error receiving data from client");
    } else {
        buffer[bytes_received] = '\0';  // Kết thúc chuỗi nhận được
    }
    return bytes_received;
}

// Add this function before handle_client_connection
int receive_message(int client_fd, char* buffer, int buffer_size) {
    return receive_data(client_fd, buffer, buffer_size);
}

// Đóng kết nối với client
void close_connection(int client_fd) {
    close(client_fd);
    log_info("Closed connection with client");
}

// Hàm xử lý kết nối client trong một vòng lặp cho server
void handle_client_connection(int server_fd) {
    while (1) {
        int client_fd = accept_client_connection(server_fd);
        if (client_fd < 0) {
            log_error("Failed to accept client connection");
            continue;
        }

        // Vòng lặp xử lý thông điệp từ client
        char buffer[BUFFER_SIZE];
        int bytes_received;
        while ((bytes_received = receive_data(client_fd, buffer, BUFFER_SIZE)) > 0) {
            handle_client_message(client_fd, buffer);  // Phân loại và xử lý thông điệp
        }

        close_connection(client_fd);  // Đóng kết nối sau khi client ngắt kết nối
    }
}
