#ifndef CONTROL_MESSAGE_HANDLER_H
#define CONTROL_MESSAGE_HANDLER_H

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

// Hàm gửi thông điệp đăng nhập tới server
void send_login(int client_fd, const char* username, const char* password);

// Hàm gửi thông điệp đăng ký tới server
void send_register(int client_fd, const char* username, const char* password);

// Hàm gửi thông điệp tạo dự án tới server
void send_create_project(int client_fd, const char* project_name, const char* description, int created_by);

// Hàm gửi thông điệp mời thành viên vào dự án
void send_add_member(int client_fd, int project_id, int user_id, const char *role);

// Hàm xử lý thông điệp control từ server
void handle_control_message(int client_fd, const char* message);

#endif // CONTROL_MESSAGE_HANDLER_H