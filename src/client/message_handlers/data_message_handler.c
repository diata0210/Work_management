#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "data_message_handler.h"
#include "../network/socket.h"  // Giả sử bạn có một hàm send_data()
#include "../logger/logger.h"

// Hàm gửi thông điệp cập nhật trạng thái task tới server
void send_task_update(int client_fd, int task_id, const char* status) {
    char task_update_message[512];

    // Tạo thông điệp cập nhật trạng thái task theo định dạng: TASK_UPDATE <task_id> <status>
    snprintf(task_update_message, sizeof(task_update_message), "TASK_UPDATE %d %s", task_id, status);

    // Gửi thông điệp cập nhật trạng thái task tới server
    if (send(client_fd, task_update_message, strlen(task_update_message), 0) == -1) {
        perror("send_task_update() failed");
    } else {
        printf("Task update message sent to server: %s\n", task_update_message);
    }
}

// Hàm gửi thông điệp thêm bình luận vào task tới server
void send_add_comment(int client_fd, int task_id, int user_id, const char* comment) {
    char add_comment_message[512];

    // Tạo thông điệp thêm bình luận vào task theo định dạng: ADD_COMMENT <task_id> <user_id> <comment>
    snprintf(add_comment_message, sizeof(add_comment_message), "ADD_COMMENT %d %d %s", task_id, user_id, comment);

    // Gửi thông điệp thêm bình luận vào task tới server
    if (send(client_fd, add_comment_message, strlen(add_comment_message), 0) == -1) {
        perror("send_add_comment() failed");
    } else {
        printf("Add comment message sent to server: %s\n", add_comment_message);
    }
}

// Hàm gửi thông điệp đính kèm tệp vào task tới server
void send_file_attachment(int client_fd, int task_id, const char* filename, const char* file_path, int uploaded_by) {
    char file_attachment_message[512];

    // Tạo thông điệp đính kèm tệp vào task theo định dạng: FILE_ATTACHMENT <task_id> <filename> <file_path> <uploaded_by>
    snprintf(file_attachment_message, sizeof(file_attachment_message), 
             "FILE_ATTACHMENT %d %s %s %d", task_id, filename, file_path, uploaded_by);

    // Gửi thông điệp đính kèm tệp tới server
    if (send(client_fd, file_attachment_message, strlen(file_attachment_message), 0) == -1) {
        perror("send_file_attachment() failed");
    } else {
        printf("File attachment message sent to server: %s\n", file_attachment_message);
    }
}

// Hàm xử lý thông điệp data từ server
void handle_data_message(int client_fd, const char* message) {
    char command[20];
    sscanf(message, "%s", command);

    if (strcmp(command, "TASK_UPDATE") == 0) {
        int task_id;
        char status[50];
        sscanf(message + 12, "%d %s", &task_id, status); // Bỏ qua "TASK_UPDATE "
        send_task_update(client_fd, task_id, status);
    } else if (strcmp(command, "ADD_COMMENT") == 0) {
        int task_id, user_id;
        char comment[500];
        sscanf(message + 11, "%d %d %[^\n]", &task_id, &user_id, comment); // Bỏ qua "ADD_COMMENT "
        send_add_comment(client_fd, task_id, user_id, comment);
    } else if (strcmp(command, "FILE_ATTACHMENT") == 0) {
        int task_id, uploaded_by;
        char filename[100], file_path[200];
        sscanf(message + 15, "%d %99s %199s %d", &task_id, filename, file_path, &uploaded_by); // Bỏ qua "FILE_ATTACHMENT "
        send_file_attachment(client_fd, task_id, filename, file_path, uploaded_by);
    } else {
        log_error("Unknown data message: %s", message);
    }
}
