#ifndef CHAT_MESSAGE_HANDLER_H
#define CHAT_MESSAGE_HANDLER_H

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

// Hàm gửi tin nhắn chat tới một thành viên trong nhóm
void send_chat_to_member(int client_fd, int member_fd, const char* message);

// Hàm gửi tin nhắn chat tới tất cả các thành viên trong nhóm
void send_chat_to_group(int client_fd, int* member_fds, int member_count, const char* message);

// Hàm xử lý thông điệp chat từ server (có thể là tin nhắn chat hoặc các hành động khác)
void handle_chat_message(int client_fd, const char* message);

// Hàm gửi tin nhắn chat từ client tới server
void send_chat_message(int client_fd, const char* message);

#endif // CHAT_MESSAGE_HANDLER_H
