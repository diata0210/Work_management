#ifndef DATA_MESSAGE_HANDLER_H
#define DATA_MESSAGE_HANDLER_H

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

// Hàm gửi thông điệp cập nhật trạng thái task tới server
void send_task_update(int client_fd, int task_id, const char* status);

// Hàm gửi thông điệp thêm bình luận vào task tới server
void send_add_comment(int client_fd, int task_id, int user_id, const char* comment);

// Hàm gửi thông điệp đính kèm tệp vào task tới server
void send_file_attachment(int client_fd, int task_id, const char* filename, const char* file_path, int uploaded_by);

// Hàm xử lý thông điệp data từ server
void handle_data_message(int client_fd, const char* message);

#endif // DATA_MESSAGE_HANDLER_H
