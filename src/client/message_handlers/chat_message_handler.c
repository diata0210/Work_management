#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <stdbool.h>
#include "chat_message_handler.h"
#include "../network/socket.h"  // Giả sử bạn có một hàm send_chat_to_member()




bool send_project_chat(int client_fd,int project_id, const char* content) {
    char chat_message[512];
    char response_buffer[1024];
    memset(response_buffer, 0, 1024);
    // Tạo thông điệp chat theo định dạng: CHAT <project_id> <content>
    snprintf(chat_message, sizeof(chat_message), "CHAT %d %s", project_id, content);
    printf("%s",chat_message);
    // Gửi thông điệp chat tới thành viên
    if (send_request(chat_message, response_buffer)) {
        if (strcmp(response_buffer,"CHAT_SENT") == 0){
            return true;
        }
    }
    return false;
}


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
// void send_chat_to_group(int client_fd, int* member_fds, int member_count, const char* message) {
//     for (int i = 0; i < member_count; i++) {
//         if (member_fds[i] != client_fd) {  // Không gửi lại cho chính người gửi
//             send_chat_to_member(client_fd, member_fds[i], message);
//         }
//     }
// }

// Hàm xử lý thông điệp chat từ server (có thể là tin nhắn chat hoặc các hành động khác)


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
