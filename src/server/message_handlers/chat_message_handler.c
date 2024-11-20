#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "chat_message_handler.h"
#include "logger.h"

void send_chat_to_member(int member_fd, const char* message) {
    if (send(member_fd, message, strlen(message), 0) == -1) {
        log_error("Failed to send chat message to member with fd %d", member_fd);
    }
}

// Tìm danh sách thành viên trong nhóm chat
int* find_group_members(int project_id, int* member_count) {
    *member_count = 0;  // Ví dụ để trả về số lượng thành viên thực tế sau khi tìm kiếm
    return NULL;         // Thay đổi logic để trả về danh sách thành viên thực tế
}

void handle_chat_message(int client_fd, const char* message) {
    int project_id;        
    char chat_content[256]; // Nội dung tin nhắn (sau khi xử lý từ `message`)

    int member_count;
    int* member_fds = find_group_members(project_id, &member_count);

    if (member_fds == NULL || member_count == 0) {
        log_error("No members found in project %d for chat", project_id);
        return;
    }

    for (int i = 0; i < member_count; i++) {
        if (member_fds[i] != client_fd) { // Không gửi lại cho chính người gửi
            send_chat_to_member(member_fds[i], chat_content);
        }
    }

    free(member_fds);
}
