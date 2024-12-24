#include "socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>

#define BUFFER_SIZE 1024

static int sock_fd = -1;    // File descriptor của socket

// Hàm khởi tạo socket
bool initialize_socket(const char *server_ip, int port) {
    if (sock_fd >= 0) {
        fprintf(stderr, "Socket already initialized.\n");
        return true; // Socket đã được khởi tạo
    }

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("Socket creation failed");
        return false;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        close_socket();
        return false;
    }

    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close_socket();
        return false;
    }

    printf("Socket connected to %s:%d\n", server_ip, port);
    return true;
}

// Hàm đóng socket
void close_socket() {
    if (sock_fd >= 0) {
        close(sock_fd);
        sock_fd = -1;
        printf("Socket closed.\n");
    }
}

// Hàm gửi yêu cầu với phản hồi
bool send_request(const char *request, char *response) {
    if (sock_fd < 0) {
        fprintf(stderr, "Error: Socket is not initialized.\n");
        return false;
    }

    if (send(sock_fd, request, strlen(request), 0) < 0) {
        perror("Send failed");
        close_socket();
        return false;
    }

    if (response) {
        if (!receive_response(response, BUFFER_SIZE)) {
            fprintf(stderr, "Error: Failed to receive response.\n");
            return false;
        }
    }

    return true;
}

bool receive_response(char *response, int max_length) {
    int bytes_received = recv(sock_fd, response, max_length - 1, 0);
    if (bytes_received < 0) {
        perror("Receive failed");
        close_socket();
        return false;
    } else if (bytes_received == 0) {
        fprintf(stderr, "Error: Connection closed by server\n");
        close_socket();
        return false;
    }

    response[bytes_received] = '\0';
    return true;
}


// Hàm nhận dữ liệu không đồng bộ
bool receive_data_async(void (*callback)(const char *data)) {
    if (sock_fd < 0) {
        fprintf(stderr, "Socket is not initialized.\n");
        return false;
    }

    pthread_t thread;
    if (pthread_create(&thread, NULL, (void *(*)(void *))start_receive_loop, callback) != 0) {
        perror("Failed to create receive thread");
        return false;
    }

    pthread_detach(thread); // Tự động giải phóng luồng sau khi kết thúc
    return true;
}

// Vòng lặp nhận dữ liệu
void *start_receive_loop(void *callback) {
    void (*handle_data)(const char *) = callback;
    char buffer[BUFFER_SIZE];

    while (true) {
        int bytes_received = recv(sock_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) {
            if (bytes_received == 0) {
                fprintf(stderr, "Connection closed by server\n");
            } else {
                perror("Receive error");
            }
            close_socket();
            break; // Thoát vòng lặp khi có lỗi
        }

        buffer[bytes_received] = '\0';
        // Gọi callback để xử lý dữ liệu nhận được
        handle_data(buffer);
    }

    return NULL;
}

// Hàm gửi yêu cầu không cần phản hồi
bool send_request_no_response(const char *request) {
    if (sock_fd < 0) {
        fprintf(stderr, "Socket is not initialized.\n");
        return false;
    }

    if (send(sock_fd, request, strlen(request), 0) < 0) {
        perror("Send failed");
        close_socket();
        return false;
    }

    return true;
}
