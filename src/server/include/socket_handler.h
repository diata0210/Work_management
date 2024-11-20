#ifndef SOCKET_HANDLER_H
#define SOCKET_HANDLER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Khởi tạo server socket
int setup_server_socket(int port);

// Chấp nhận kết nối từ client
int accept_client_connection(int server_fd);

// Gửi dữ liệu tới client
int send_data(int client_fd, const char* data);

// Nhận dữ liệu từ client
int receive_data(int client_fd, char* buffer, int buffer_size);

// Đóng kết nối với client
void close_connection(int client_fd);

// Hàm xử lý client
void handle_client(int client_fd);

#endif // SOCKET_HANDLER_H
