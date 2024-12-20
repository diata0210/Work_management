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
#include "user_dao.h"
#define BUFFER_SIZE MAX_BUFFER

#include <pthread.h> // Thêm để sử dụng mutex

#define MAX_USERS 100  // Giới hạn số lượng người dùng

extern sqlite3 *db;

typedef struct {
    int userid;
    int client_fd;
} UserSocket;

UserSocket user_sockets[MAX_USERS];
pthread_mutex_t user_sockets_mutex = PTHREAD_MUTEX_INITIALIZER;

// Khởi tạo mảng user_sockets
void initialize_user_sockets() {
    for (int i = 0; i < MAX_USERS; i++) {
        user_sockets[i].userid = -1;
        user_sockets[i].client_fd = -1;
    }
}



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

int find_userid_by_client_fd(int client_fd) {
    pthread_mutex_lock(&user_sockets_mutex);
    for (int i = 0; i < MAX_USERS; i++) {
        if (user_sockets[i].client_fd == client_fd) {
            int userid = user_sockets[i].userid;
            pthread_mutex_unlock(&user_sockets_mutex);
            return userid;
        }
    }
    pthread_mutex_unlock(&user_sockets_mutex);
    return -1;  // Không tìm thấy
}


int find_client_fd_by_userid(int userid) {
    pthread_mutex_lock(&user_sockets_mutex);
    for (int i = 0; i < MAX_USERS; i++) {
        if (user_sockets[i].userid == userid) {
            int client_fd = user_sockets[i].client_fd;
            pthread_mutex_unlock(&user_sockets_mutex);
            return client_fd;
        }
    }
    pthread_mutex_unlock(&user_sockets_mutex);
    return -1;  // Không tìm thấy
}
int add_user_socket(int userid, int client_fd) {
    pthread_mutex_lock(&user_sockets_mutex);
    for (int i = 0; i < MAX_USERS; i++) {
        if (user_sockets[i].userid == -1) {  // Vị trí trống
            user_sockets[i].userid = userid;
            user_sockets[i].client_fd = client_fd;
            pthread_mutex_unlock(&user_sockets_mutex);
            return 0;  // Thành công
        }
    }
    pthread_mutex_unlock(&user_sockets_mutex);
    return -1;  // Mảng đầy
}
void remove_user_socket(int client_fd) {
    pthread_mutex_lock(&user_sockets_mutex);
    for (int i = 0; i < MAX_USERS; i++) {
        if (user_sockets[i].client_fd == client_fd) {
            user_sockets[i].userid = -1;
            user_sockets[i].client_fd = -1;
            break;
        }
    }
    pthread_mutex_unlock(&user_sockets_mutex);
}

void handle_login(int client_fd, const char* username, const char* password) {
    int userid = login_user(db, username, password);
    if (userid != -1) {
        if (add_user_socket(userid, client_fd) == 0) {
            send_data(client_fd, "LOGIN_SUCCESS");
            log_info("User %s (ID: %d) logged in successfully", username, userid);
        } else {
            send_data(client_fd, "LOGIN_FAILED_NO_SPACE");
            log_error("No space to store user %s (ID: %d)", username, userid);
        }
    } else {
        send_data(client_fd, "LOGIN_FAILED");
        log_error("Login failed for user %s", username);
    }
}



// Phân loại và xử lý các thông điệp từ client
void handle_client_message(int client_fd, const char* message) {
    // Tìm userid dựa trên client_fd
    int userid = find_userid_by_client_fd(client_fd);

    // Xử lý thông điệp CONTROL (không cần kiểm tra trạng thái đăng nhập)
    if (strncmp(message, "CONTROL", 7) == 0) {
        // Nếu là lệnh LOGIN, xử lý đăng nhập và gán client_fd với userid
        if (strncmp(message + 8, "LOGIN", 5) == 0) {
            char username[50], password[50];
            sscanf(message + 14, "%s %s", username, password); // Bỏ qua "CONTROL LOGIN "
            handle_login(client_fd, username, password); // Xử lý đăng nhập
        } else {
            handle_control_message(client_fd,userid, message + 8); // Các lệnh CONTROL khác
        }
        return;
    }
    
    
    // Kiểm tra xem client_fd có được gắn userid hay chưa (đã đăng nhập hay chưa)
    if (userid == -1) {
        // Nếu chưa đăng nhập, từ chối xử lý các loại thông điệp khác
        send_data(client_fd, "NOT_LOGGED_IN");
        log_error("Unauthorized access attempt by client_fd %d", client_fd);
        return;
    }

    // Xử lý các thông điệp khác
    if (strncmp(message, "DATA", 4) == 0) {
        handle_data_message(client_fd, userid, message + 5);  // Gọi đến handler dữ liệu
    } else if (strncmp(message, "CHAT", 4) == 0) {
        handle_chat_message(client_fd, userid, message + 5);  // Gọi đến handler chat
    } else {
        log_error("Unknown message type from userid %d: %s", userid, message);
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
