#include <stdio.h>
#include <string.h>

#include "menu.h"
#include "control_message_handler.h"
#include "chat_message_handler.h"
#include "data_message_handler.h"
#include "video_message_handler.h"
#include "socket.h"
int receive_data(int client_fd, char *buffer, int buffer_size) {
    int bytes_received = recv(client_fd, buffer, buffer_size - 1, 0);
    if (bytes_received == -1) {
        perror("Error receiving message");
        return -1;
    }
    buffer[bytes_received] = '\0';  // Kết thúc chuỗi
    return bytes_received;
}
void display_menu() {
    printf("==== Client Menu ====\n");
    printf("1. Control\n");
    printf("2. Data\n");
    printf("3. Chat\n");
    printf("4. Video\n");
    printf("5. Exit\n");
    printf("Select an option: ");
}

void send_login(int client_fd, const char* username, const char* password);
void send_register(int client_fd, const char* username, const char* password);
void send_create_project(int client_fd, const char* project_name, const char* description, int created_by);
void send_add_member(int client_fd, int project_id, int user_id, const char* role);


void test_control(int client_fd) {
    int choice;
    printf("\nControl Test Menu\n");
    printf("1. Login\n");
    printf("2. Register\n");
    printf("3. Create Project\n");
    printf("4. Add Member\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    getchar(); // Clear input buffer

    char buffer[1024];  // Chỉ khai báo buffer một lần duy nhất

    switch (choice) {
        case 1: {
            // Test Login
            char username[50], password[50];
            printf("Enter username: ");
            fgets(username, sizeof(username), stdin);
            username[strcspn(username, "\n")] = 0; // Remove newline character

            printf("Enter password: ");
            fgets(password, sizeof(password), stdin);
            password[strcspn(password, "\n")] = 0; // Remove newline character

            send_login(client_fd, username, password);
            int bytes_received = receive_data(client_fd, buffer, sizeof(buffer));
            if (bytes_received > 0) {
                printf("Server response: %s\n", buffer);  // In ra phản hồi từ server
            }
            break;
        }
        case 2: {
            // Test Register
            char username[50], password[50];
            printf("Enter username: ");
            fgets(username, sizeof(username), stdin);
            username[strcspn(username, "\n")] = 0; // Remove newline character

            printf("Enter password: ");
            fgets(password, sizeof(password), stdin);
            password[strcspn(password, "\n")] = 0; // Remove newline character

            send_register(client_fd, username, password);
            int bytes_received = receive_data(client_fd, buffer, sizeof(buffer));
            if (bytes_received > 0) {
                printf("Server response: %s\n", buffer);  // In ra phản hồi từ server
            }
            break;
        }
        case 3: {
            // Test Create Project
            char project_name[100], description[255];
            int created_by;
            printf("Enter project name: ");
            fgets(project_name, sizeof(project_name), stdin);
            project_name[strcspn(project_name, "\n")] = 0; // Remove newline character

            printf("Enter project description: ");
            fgets(description, sizeof(description), stdin);
            description[strcspn(description, "\n")] = 0; // Remove newline character

            printf("Enter user ID for creator: ");
            scanf("%d", &created_by);
            getchar(); // Clear input buffer

            send_create_project(client_fd, project_name, description, created_by);
            int bytes_received = receive_data(client_fd, buffer, sizeof(buffer));
            if (bytes_received > 0) {
                printf("Server response: %s\n", buffer);  // In ra phản hồi từ server
            }
            break;
        }
        case 4: {
            // Test Add Member
            int project_id, user_id;
            char role[20];
            printf("Enter project ID: ");
            scanf("%d", &project_id);

            printf("Enter user ID: ");
            scanf("%d", &user_id);
            getchar(); // Clear input buffer

            printf("Enter role: ");
            fgets(role, sizeof(role), stdin);
            role[strcspn(role, "\n")] = 0; // Remove newline character

            send_add_member(client_fd, project_id, user_id, role);
            int bytes_received = receive_data(client_fd, buffer, sizeof(buffer));
            if (bytes_received > 0) {
                printf("Server response: %s\n", buffer);  // In ra phản hồi từ server
            }
            break;
        }
        default:
            printf("Invalid choice! Please select again.\n");
            break;
    }
}


// Test phần data
// void test_data(int client_fd) {
//     int choice;
//     printf("\nData Test Menu\n");
//     printf("1. Update Task Status\n");
//     printf("2. Add Comment to Task\n");
//     printf("3. Attach File to Task\n");
//     printf("Enter your choice: ");
//     scanf("%d", &choice);
//     getchar(); // Clear input buffer

//     switch (choice) {
//         case 1: {
//             // Test Cập nhật trạng thái công việc
//             int task_id;
//             char status[50];
//             printf("Enter task ID: ");
//             scanf("%d", &task_id);
//             getchar(); // Clear input buffer

//             printf("Enter task status (e.g., In Progress, Completed): ");
//             fgets(status, sizeof(status), stdin);
//             status[strcspn(status, "\n")] = 0; // Remove newline character

//             send_task_update(client_fd, task_id, status);
//             int bytes_received = receive_data(client_fd, buffer, sizeof(buffer));
//             if (bytes_received > 0) {
//                 printf("Server response: %s\n", buffer);  // In ra phản hồi từ server
//             }
//             break;
            
//         }
//         case 2: {
//             // Test Thêm bình luận vào công việc
//             int task_id, user_id;
//             char comment[500];
//             printf("Enter task ID: ");
//             scanf("%d", &task_id);

//             printf("Enter user ID: ");
//             scanf("%d", &user_id);
//             getchar(); // Clear input buffer

//             printf("Enter comment: ");
//             fgets(comment, sizeof(comment), stdin);
//             comment[strcspn(comment, "\n")] = 0; // Remove newline character

//             send_add_comment(client_fd, task_id, user_id, comment);
//             receive_data(client_fd);
//             break;
//         }
//         case 3: {
//             // Test Đính kèm tệp vào công việc
//             int task_id, uploaded_by;
//             char filename[100], file_path[200];
//             printf("Enter task ID: ");
//             scanf("%d", &task_id);

//             printf("Enter filename: ");
//             getchar(); // Clear input buffer
//             fgets(filename, sizeof(filename), stdin);
//             filename[strcspn(filename, "\n")] = 0; // Remove newline character

//             printf("Enter file path: ");
//             fgets(file_path, sizeof(file_path), stdin);
//             file_path[strcspn(file_path, "\n")] = 0; // Remove newline character

//             printf("Enter user ID for uploader: ");
//             scanf("%d", &uploaded_by);
//             getchar(); // Clear input buffer

//             send_file_attachment(client_fd, task_id, filename, file_path, uploaded_by);
//             receive_data(client_fd);
//             break;
//         }
//         default:
//             printf("Invalid choice! Please select again.\n");
//             break;
//     }
// }


// void test_chat(int client_fd) {
//     int choice;
//     printf("\nChat Test Menu\n");
//     printf("1. Send Message to Group\n");
//     printf("2. Send Message to Member\n");
//     printf("Enter your choice: ");
//     scanf("%d", &choice);
//     getchar(); // Clear input buffer

//     switch (choice) {
//         case 1: {
//             // Test gửi tin nhắn tới nhóm
//             // int project_id;
//             // char message[256];
//             // printf("Enter project ID: ");
//             // scanf("%d", &project_id);
//             // getchar(); // Clear input buffer

//             // printf("Enter message: ");
//             // fgets(message, sizeof(message), stdin);
//             // message[strcspn(message, "\n")] = 0; // Remove newline character

//             // send_chat_to_group(client_fd, project_id, message);
//             // receive_data(client_fd);  // Nhận phản hồi từ server
//             // break;
//         }
//         case 2: {
//             // Test gửi tin nhắn tới một thành viên cụ thể
//             int member_fd;
//             char message[256];
//             printf("Enter member FD (file descriptor): ");
//             scanf("%d", &member_fd);

//             getchar(); // Clear input buffer
//             printf("Enter message: ");
//             fgets(message, sizeof(message), stdin);
//             message[strcspn(message, "\n")] = 0; // Remove newline character

//             send_chat_to_member(client_fd,member_fd, message);
//             receive_data(client_fd);  // Nhận phản hồi từ server
//             break;
//         }
//         default:
//             printf("Invalid choice! Please select again.\n");
//             break;
//     }
// }



// // Test phần video
// void test_video(int client_fd) {
//     // int choice;
//     // printf("\nVideo Test Menu\n");
//     // printf("1. Send OFFER\n");
//     // printf("2. Send ANSWER\n");
//     // printf("3. Send ICE Candidate\n");
//     // printf("Enter your choice: ");
//     // scanf("%d", &choice);
//     // getchar(); // Clear input buffer

//     // switch (choice) {
//     //     case 1: {
//     //         // Test Gửi OFFER
//     //         char offer[500];
//     //         printf("Enter OFFER message: ");
//     //         fgets(offer, sizeof(offer), stdin);
//     //         offer[strcspn(offer, "\n")] = 0; // Remove newline character

//     //         send_video_offer(client_fd, offer);
//     //         receive_data(client_fd);
//     //         break;
//     //     }
//     //     case 2: {
//     //         // Test Gửi ANSWER
//     //         char answer[500];
//     //         printf("Enter ANSWER message: ");
//     //         fgets(answer, sizeof(answer), stdin);
//     //         answer[strcspn(answer, "\n")] = 0; // Remove newline character

//     //         send_video_answer(client_fd, answer);
//     //         receive_data(client_fd);
//     //         break;
//     //     }
//     //     case 3: {
//     //         // Test Gửi ICE Candidate
//     //         char candidate[500];
//     //         printf("Enter ICE Candidate message: ");
//     //         fgets(candidate, sizeof(candidate), stdin);
//     //         candidate[strcspn(candidate, "\n")] = 0; // Remove newline character

//     //         send_video_candidate(client_fd, candidate);
//     //         receive_data(client_fd);
//     //         break;
//     //     }
//     //     default:
//     //         printf("Invalid choice! Please select again.\n");
//     //         break;
//     // }
// }
