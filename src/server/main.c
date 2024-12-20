// main.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include "socket_handler.h"
#include "config.h"
#include "logger.h"
#include "db_init.h"  // Bao gồm db_handler để sử dụng các hàm DB

void sigchld_handler(int sig) {
    (void)sig;  
    while (waitpid(-1, NULL, WNOHANG) > 0); 
}

int main(int argc, char *argv[]) {
    // Kiểm tra nếu không có đối số cổng
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    // Chuyển đổi đối số cổng từ chuỗi sang số nguyên
    int server_port = atoi(argv[1]);

    // Kiểm tra nếu cổng không hợp lệ (ví dụ, nếu cổng <= 1024 hoặc không phải số)
    if (server_port <= 1024 || server_port > 65535) {
        fprintf(stderr, "Invalid port number. Please choose a port between 1025 and 65535.\n");
        exit(1);
    }

    // Mở kết nối tới cơ sở dữ liệu
    if (initialize_database("/home/parallels/Work_management-server/resources/database/project.db") != 0) {  // Đảm bảo sử dụng đường dẫn đúng tới DB
        log_error("Failed to open database.");
        exit(1);
    }

    // Thiết lập hàm xử lý tín hiệu SIGCHLD
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART; 
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    // Thiết lập socket cho server
    int server_fd = setup_server_socket(server_port);
    if (server_fd < 0) {
        log_error("Failed to set up server socket");
        return 1;
    }

    log_info("Server is running on port %d...", server_port);
    initialize_user_sockets();
    // Vòng lặp chính để chấp nhận các kết nối client
    while (1) {
        int client_fd = accept_client_connection(server_fd);
        
        if (client_fd < 0) {
            log_error("Failed to accept client connection");
            continue;
        }

        pid_t pid = fork();
        if (pid == 0) {  // Tiến trình con
    close(server_fd);  // Đóng server_fd trong tiến trình con

    char buffer[MAX_BUFFER];
    int bytes_received;

    // Vòng lặp để nhận và xử lý thông điệp từ client
    while ((bytes_received = receive_message(client_fd, buffer, MAX_BUFFER)) > 0) {
        handle_client_message(client_fd, buffer);
    }

    if (bytes_received == 0) {
        log_info("Client disconnected");
    } else if (bytes_received < 0) {
        log_error("Error receiving data from client");
    }

    close_connection(client_fd);
    exit(0);  // Kết thúc tiến trình con
} else if (pid > 0) {  // Tiến trình cha
            close(client_fd);  // Đóng client_fd trong tiến trình cha
        } else {
            log_error("Failed to fork process for client");
            close_connection(client_fd);
        }
    }

    // Đóng kết nối DB khi server dừng
    close_database();
    close(server_fd);  // Đóng server_fd khi server kết thúc
    return 0;
}
