#ifndef DATA_MESSAGE_HANDLER_H
#define DATA_MESSAGE_HANDLER_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>

// Include các thư viện và module liên quan
#include "data_message_handler.h"
#include "../network/socket.h"
#include "../logger/logger.h"

// Hàm gửi yêu cầu lấy danh sách task của dự án
bool send_get_tasks_request(int project_id, char *response_buffer, int buffer_size);

// Hàm gửi yêu cầu tạo task mới
bool send_create_task(const char* task_name, const char* description, int project_id, int assignee_id);

// Hàm gửi thông điệp cập nhật trạng thái task tới server
void send_task_update(int client_fd, int task_id, const char* status);
bool send_add_comment(int task_id, const char *content);
// bool send_add_attachment(int task_id, const char *file_name, const char *file_path) ;
bool send_add_attachment(int task_id, const char *file_name, const char *file_path);
bool send_get_comments_request(int task_id, char *response, size_t response_size);
bool send_get_attachments_request(int task_id, char *response, size_t response_size);

bool send_update_status_request(int task_id, const char *status) ;



// Hàm gửi thông điệp thêm bình luận vào task tới server
// void send_add_comment(int client_fd, int task_id, int user_id, const char* comment);

// // Hàm gửi thông điệp đính kèm tệp vào task tới server
// void send_file_attachment(int client_fd, int task_id, const char* filename, const char* file_path, int uploaded_by);

// Hàm xử lý thông điệp "DATA" từ server

#endif 
