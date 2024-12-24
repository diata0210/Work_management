#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "chat_message_handler.h"
#include "socket_handler.h"
#include "project_dao.h"
#include "logger.h"
extern sqlite3 *db;
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

// void handle_chat_message(int client_fd, int user_id,  const char* message) {
//     int project_id;        
//     char chat_content[256]; // Nội dung tin nhắn (sau khi xử lý từ `message`)

//     int member_count;
//     int* member_fds = find_group_members(project_id, &member_count);

//     if (member_fds == NULL || member_count == 0) {
//         log_error("No members found in project %d for chat", project_id);
//         return;
//     }

//     for (int i = 0; i < member_count; i++) {
//         if (member_fds[i] != client_fd) { // Không gửi lại cho chính người gửi
//             send_chat_to_member(member_fds[i], chat_content);
//         }
//     }

//     free(member_fds);
// }


// void handle_chat_message(int client_fd, int sender_user_id, const char *message) {
//     int project_id;
//     char content[2048];
    

//     // Parse "CHAT project_id content"
//     if (sscanf(message, "%d %[^\n]", &project_id, content) < 2) {
//         send_data(client_fd, "INVALID_CHAT_MESSAGE_FORMAT");
//         return;
//     }

//     // Fetch members in the project
//     UserArray members = get_project_members(db, project_id);
//     if (members.user_ids == NULL) {
//         send_data(client_fd, "FAILED_TO_FETCH_PROJECT_MEMBERS");
//         return;
//     }
//     printf("%d",members.count);
//     // Send the message to all members except the sender
//     for (int i = 0; i < members.count; i++) {
        
//         printf("User id: %d\n", members.user_ids[i]);
        
//             int member_fd = find_client_fd_by_userid(members.user_ids[i]);
//             printf("Member fd: %d\n", member_fd);

//             if (member_fd != -1) {
//                 char full_message[2048];
//                 snprintf(full_message, sizeof(full_message), "SERVER CHAT %d %s", project_id, content);
//                 printf("%s",full_message);
//                 send_data(member_fd, full_message);
//             }
        
//     }

//     free_user_array(&members);
// }
