// #ifndef CLIENT_H
// #define CLIENT_H

// #define SERVER_IP "127.0.0.1"  // Địa chỉ IP của server
// #define SERVER_PORT 8080       // Cổng kết nối của server





// #include <gtk/gtk.h>
// #include <gst/gst.h>
// #include <arpa/inet.h>
// #include <string.h>
// #include <stdlib.h>
// #include <stdio.h>
// #include <unistd.h>

// // Struct lưu thông tin người dùng
// typedef struct {
//     int id;
//     char username[50];
//     gboolean is_logged_in;
// } UserInfo;

// // Biến toàn cục
// extern int client_socket;
// extern UserInfo current_user;

// // Hàm khởi tạo ứng dụng client
// void init_client(const char *server_ip, int server_port);

// // Hàm kết nối tới server
// int connect_to_server();

// // Hàm gửi dữ liệu tới server
// int send_data(int server_fd, const char* data);

// // Hàm nhận dữ liệu từ server
// int receive_data(int server_fd, char* buffer, int buffer_size);

// // Hàm xử lý thông điệp từ server
// void handle_server_message(const char* message);

// #endif
// src/client/include/client.h

#ifndef CLIENT_H
#define CLIENT_H

#include <gtk/gtk.h>

typedef struct {
    int socket_fd;
    int user_id;
    char *username;
    GtkWidget *main_window;
    GtkWidget *chat_window;
    GtkWidget *video_window;
    gboolean is_logged_in;
} Client;

// Khởi tạo client
Client* client_init();

// Kết nối tới server
int client_connect(Client *client, const char *host, int port);

// Đóng kết nối và giải phóng tài nguyên
void client_cleanup(Client *client);

// Xử lý message từ server
void client_handle_message(Client *client, const char *message);

// Gửi message tới server
int client_send_message(Client *client, const char *message);

#endif // CLIENT_H
