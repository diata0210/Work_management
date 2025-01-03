#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "data_message_handler.h"
#include "../network/socket.h"  // Giả sử bạn có một hàm send_data()
#include "../logger/logger.h"



bool send_get_tasks_request(int project_id, char *response_buffer, int buffer_size) {
    // Tạo yêu cầu GET_TASK
    char request[256];
    snprintf(request, sizeof(request), "DATA GET_TASK %d", project_id);

    // Xóa dữ liệu cũ trong buffer
    memset(response_buffer, 0, buffer_size);

    // Gửi yêu cầu và nhận phản hồi từ server
    if (send_request(request, response_buffer)) {
        return true;
    }

    // Nếu gửi yêu cầu thất bại
    fprintf(stderr, "Failed to send GET_TASK request for project ID: %d\n", project_id);
    return false;
}

bool send_create_task( const char* task_name, const char* description, int project_id, int assignee_id) {
    char create_project_message[512];
    char response_buffer[1024];
    memset(response_buffer, 0, 1024);
    // Tạo thông điệp tạo dự án theo định dạng: CREATE_PROJECT <project_name> , <description> <created_by>
    snprintf(create_project_message, sizeof(create_project_message), 
             "DATA CREATE_TASK %s %s %d %d", task_name, description, project_id, assignee_id);
    printf("%s",create_project_message);
    if (send_request(create_project_message, response_buffer)) {
        if (strcmp(response_buffer, "TASK_CREATED") == 0){
            return true;
        }
    }

    fprintf(stderr, "Failed to send CREATE_TASK request.\n");
    return false;
    
}

bool send_get_comments_request(int task_id, char *response, size_t response_size) {
    char request[256];
    snprintf(request, sizeof(request), "DATA GET_COMMENTS %d", task_id);
    return send_request(request, response) && strncmp(response, "COMMENTS", 8) == 0;
}






bool send_get_attachments_request(int task_id, char *response, size_t response_size) {
    char request[256];
    snprintf(request, sizeof(request), "DATA GET_ATTACHMENTS% d", task_id);
    return send_request(request, response) && strncmp(response, "ATTACHMENTS", 11) == 0;
}
#define BUFFER_SIZE 1024

// bool send_add_attachment(int task_id, const char *file_name, const char *file_path) {
//     // Mở file ở chế độ đọc nhị phân
//     FILE *file = fopen(file_path, "rb");
//     if (file == NULL) {
//         log_error("Failed to open file: %s", file_path);
//         return false;
//     }

//     // Gửi metadata
//     char metadata[BUFFER_SIZE];
//     snprintf(metadata, sizeof(metadata), "DATA ADD_ATTACHMENT %d %s", task_id, file_name);
//     if (!send_request_no_response(metadata)) {
//         log_error("Failed to send metadata: %s", metadata);
//         fclose(file);
//         return false;
//     }

//     // Gửi nội dung file
//     char buffer[BUFFER_SIZE] = {0};
//     while (!feof(file)) {
//         size_t bytes_read = fread(buffer, 1, BUFFER_SIZE, file);
//         if (bytes_read > 0) {
//             if (!send_request_no_response(buffer)) {
//                 log_error("Failed to send file content for: %s", file_name);
//                 fclose(file);
//                 return false;
//             }
//         }
//         memset(buffer, 0, BUFFER_SIZE);
//     }
//     fclose(file);

//     log_info("File sent successfully without waiting for response: %s", file_name);
//     return true;
// }
bool send_add_attachment(int task_id, const char *file_name, const char *file_path) {
    // Mở file ở chế độ đọc nhị phân
    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        log_error("Failed to open file: %s", file_path);
        return false;
    }

    // Gửi metadata ban đầu
    char metadata[BUFFER_SIZE];
    snprintf(metadata, sizeof(metadata), "DATA ADD_ATTACHMENT %d %s", task_id, file_name);
    printf("metadata: %s\n", metadata);
    if (!send_request_no_response(metadata)) {
        log_error("Failed to send metadata: %s", metadata);
        fclose(file);
        return false;
    }
    sleep(1);
    // Gửi nội dung file theo từng gói
    char buffer[BUFFER_SIZE];
    while (!feof(file)) {
        size_t bytes_read = fread(buffer, 1, BUFFER_SIZE - 20, file); // Dành chỗ cho chuỗi "DATA ADD_ATTACHMENT "
        if (bytes_read > 0) {
            char data_packet[BUFFER_SIZE];
            snprintf(data_packet, sizeof(data_packet), "DATA ADD_ATTACHMENT ");
            memcpy(data_packet + strlen(data_packet), buffer, bytes_read);

            if (!send_request_no_response(data_packet)) {
                log_error("Failed to send file content chunk for: %s", file_name);
                fclose(file);
                return false;
            }
        }
    }
    fclose(file);

    // Gửi tín hiệu hoàn tất
    if (!send_request_no_response("DATA ADD_ATTACHMENT END")) {
        log_error("Failed to send completion signal for: %s", file_name);
        return false;
    }

    log_info("File sent successfully in chunks: %s", file_name);
    return true;
}


bool send_add_comment(int task_id, const char *content) {
    char request[1024];
    snprintf(request, sizeof(request), "DATA ADD_COMMENT %d %s", task_id, content);
    char response[256];
    return send_request(request, response) && strncmp(response, "COMMENT_ADDED", 13) == 0;
}

bool send_update_status_request(int task_id, const char *status) {
    char request[256];
    snprintf(request, sizeof(request), "DATA TASK_UPDATE %d %s", task_id, status);
    char response[256];
    return send_request(request, response) && strncmp(response, "TASK_STATUS_UPDATED",19 ) == 0;
}


















// Hàm gửi thông điệp cập nhật trạng thái task tới server
// void send_task_update(int client_fd, int task_id, const char* status) {
//     char task_update_message[512];

//     // Tạo thông điệp cập nhật trạng thái task theo định dạng: TASK_UPDATE <task_id> <status>
//     snprintf(task_update_message, sizeof(task_update_message), "TASK_UPDATE %d %s", task_id, status);

//     // Gửi thông điệp cập nhật trạng thái task tới server
//     if (send(client_fd, task_update_message, strlen(task_update_message), 0) == -1) {
//         perror("send_task_update() failed");
//     } else {
//         printf("Task update message sent to server: %s\n", task_update_message);
//     }
// }


// Hàm xử lý thông điệp data từ server
// void handle_data_message(int client_fd, const char* message) {
//     char command[20];
//     sscanf(message, "%s", command);

//     if (strcmp(command, "TASK_UPDATE") == 0) {
//         int task_id;
//         char status[50];
//         sscanf(message + 12, "%d %s", &task_id, status); // Bỏ qua "TASK_UPDATE "
//         send_task_update(client_fd, task_id, status);
//     } else if (strcmp(command, "ADD_COMMENT") == 0) {
//         int task_id, user_id;
//         char comment[500];
//         sscanf(message + 11, "%d %d %[^\n]", &task_id, &user_id, comment); // Bỏ qua "ADD_COMMENT "
//         send_add_comment(client_fd, task_id, user_id, comment);
//     } else if (strcmp(command, "FILE_ATTACHMENT") == 0) {
//         int task_id, uploaded_by;
//         char filename[100], file_path[200];
//         sscanf(message + 15, "%d %99s %199s %d", &task_id, filename, file_path, &uploaded_by); // Bỏ qua "FILE_ATTACHMENT "
//         send_file_attachment(client_fd, task_id, filename, file_path, uploaded_by);
//     } else {
//         log_error("Unknown data message: %s", message);
//     }
// }
