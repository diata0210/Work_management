#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "chat_message_handler.h"
#include "../network/socket.h"  // Giả sử bạn có một hàm send_chat_to_member()

// Hàm gửi tin nhắn chat tới một thành viên trong nhóm
void send_chat_to_member(int client_fd, int member_fd, const char* message) {
    char chat_message[512];

    // Tạo thông điệp chat theo định dạng: CHAT <client_fd> <message>
    snprintf(chat_message, sizeof(chat_message), "CHAT %d %s", client_fd, message);

    // Gửi thông điệp chat tới thành viên
    if (send(member_fd, chat_message, strlen(chat_message), 0) == -1) {
        perror("send_chat_to_member() failed");
    } else {
        printf("Chat message sent to member %d: %s\n", member_fd, chat_message);
    }
}

// Hàm gửi tin nhắn chat tới tất cả các thành viên trong nhóm
void send_chat_to_group(int client_fd, int* member_fds, int member_count, const char* message) {
    for (int i = 0; i < member_count; i++) {
        if (member_fds[i] != client_fd) {  // Không gửi lại cho chính người gửi
            send_chat_to_member(client_fd, member_fds[i], message);
        }
    }
}

// Hàm xử lý thông điệp chat từ server (có thể là tin nhắn chat hoặc các hành động khác)
void handle_chat_message(int client_fd, const char* message) {
    char command[20];
    int member_count = 0;
    int* member_fds = NULL;

    // Phân tích thông điệp từ server để tìm lệnh hành động
    sscanf(message, "%s", command);

    if (strcmp(command, "CHAT") == 0) {
        int sender_fd;
        char chat_content[256];
        sscanf(message + 5, "%d %[^\n]", &sender_fd, chat_content);  // Bỏ qua "CHAT "

        // Gửi tin nhắn chat tới tất cả các thành viên trong nhóm trừ sender
        // Ví dụ: member_fds là danh sách các file descriptor của thành viên trong nhóm
        send_chat_to_group(client_fd, member_fds, member_count, chat_content);
    } else {
        log_error("Unknown chat message: %s", message);
    }
}

// Hàm gửi tin nhắn chat từ client tới server
void send_chat_message(int client_fd, const char* message) {
    char chat_message[512];

    // Tạo thông điệp chat theo định dạng: CHAT <client_fd> <message>
    snprintf(chat_message, sizeof(chat_message), "CHAT %d %s", client_fd, message);

    // Gửi thông điệp chat tới server
    if (send(client_fd, chat_message, strlen(chat_message), 0) == -1) {
        perror("send_chat_message() failed");
    } else {
        printf("Chat message sent to server: %s\n", chat_message);
    }
}
