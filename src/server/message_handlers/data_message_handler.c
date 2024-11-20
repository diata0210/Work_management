#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "data_message_handler.h"
#include "task_dao.h"
#include "logger.h"
#include "socket_handler.h"  // Để sử dụng send_data

extern sqlite3 *db;

void handle_task_update(int client_fd, int task_id, const char* status) {
    if (update_task_status(db, task_id, status) == SQLITE_OK) {
        send_data(client_fd, "TASK_STATUS_UPDATED");
        log_info("Task %d updated to status: %s", task_id, status);
    } else {
        send_data(client_fd, "TASK_STATUS_UPDATE_FAILED");
        log_error("Failed to update status for task %d", task_id);
    }
}

void handle_add_comment(int client_fd, int task_id, int user_id, const char* comment) {
    if (add_comment_to_task(db, task_id, user_id, comment) == SQLITE_OK) {
        send_data(client_fd, "COMMENT_ADDED");
        log_info("Comment added to task %d by user %d", task_id, user_id);
    } else {
        send_data(client_fd, "COMMENT_FAILED");
        log_error("Failed to add comment to task %d", task_id);
    }
}

void handle_file_attachment(int client_fd, int task_id, const char* filename, const char* file_path, int uploaded_by) {
    if (add_attachment_to_task(db, task_id, filename, file_path, uploaded_by) == SQLITE_OK) {
        send_data(client_fd, "FILE_ATTACHED");
        log_info("File %s attached to task %d by user %d", filename, task_id, uploaded_by);
    } else {
        send_data(client_fd, "FILE_ATTACHMENT_FAILED");
        log_error("Failed to attach file %s to task %d", filename, task_id);
    }
}

void handle_data_message(int client_fd, const char* message) {
    char action[50];
    int task_id, user_id, uploaded_by;
    char status[50], comment[500], filename[100], file_path[200];

    // Phân tích hành động chính từ thông điệp
    if (sscanf(message, "%49s", action) != 1) {
        log_error("Failed to parse action from message: %s", message);
        send_data(client_fd, "INVALID_MESSAGE_FORMAT");
        return;
    }

    if (strcmp(action, "TASK_UPDATE") == 0) {
        // Phân tích task_id và status
        if (sscanf(message, "TASK_UPDATE %d %49s", &task_id, status) == 2) {
            handle_task_update(client_fd, task_id, status);
        } else {
            send_data(client_fd, "INVALID_TASK_UPDATE_FORMAT");
            log_error("Invalid TASK_UPDATE format: %s", message);
        }
    } else if (strcmp(action, "ADD_COMMENT") == 0) {
        // Phân tích task_id, user_id, và comment
        if (sscanf(message, "ADD_COMMENT %d %d %[^\n]", &task_id, &user_id, comment) == 3) {
            handle_add_comment(client_fd, task_id, user_id, comment);
        } else {
            send_data(client_fd, "INVALID_ADD_COMMENT_FORMAT");
            log_error("Invalid ADD_COMMENT format: %s", message);
        }
    } else if (strcmp(action, "FILE_ATTACHMENT") == 0) {
        // Phân tích task_id, filename, file_path và uploaded_by
        if (sscanf(message, "FILE_ATTACHMENT %d %99s %199s %d", &task_id, filename, file_path, &uploaded_by) == 4) {
            handle_file_attachment(client_fd, task_id, filename, file_path, uploaded_by);
        } else {
            send_data(client_fd, "INVALID_FILE_ATTACHMENT_FORMAT");
            log_error("Invalid FILE_ATTACHMENT format: %s", message);
        }
    } else {
        send_data(client_fd, "UNKNOWN_ACTION");
        log_error("Unknown message type: %s", message);
    }
}

