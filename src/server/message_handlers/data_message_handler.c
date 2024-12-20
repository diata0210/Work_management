#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "data_message_handler.h"
#include "task_dao.h"
#include "logger.h"
#include "socket_handler.h"  // Để sử dụng send_data

extern sqlite3 *db;

void handle_create_task(int client_fd, sqlite3 *db, const char *task_name, const char *description, int project_id, int assignee_id) {
    int rc = insert_task(db, task_name, description, project_id, assignee_id);
    if (rc == SQLITE_OK) {
        send_data(client_fd, "TASK_CREATED");
        log_info("Task '%s' created successfully for project %d and assigned to user %d", task_name, project_id, assignee_id);
    } else {
        send_data(client_fd, "TASK_CREATION_FAILED");
        log_error("Failed to create task '%s' for project %d", task_name, project_id);
    }
}

// Hàm xử lý yêu cầu lấy danh sách task
void handle_get_tasks(int client_fd, sqlite3 *db, int project_id) {
    char response[1024];

    // Lấy danh sách task từ cơ sở dữ liệu
    snprintf(response, sizeof(response), "TASKS_FOR_PROJECT %d:\n", project_id);
    send_data(client_fd, response);

    char sql[256];
    sqlite3_stmt *stmt;

    snprintf(sql, sizeof(sql), "SELECT task_id, name, description, status, assignee_id, due_date FROM tasks WHERE project_id = %d;", project_id);

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        snprintf(response, sizeof(response), "Failed to fetch tasks: %s\n", sqlite3_errmsg(db));
        send_data(client_fd, response);
        log_error("%s", response);
        return;
    }

    // Gửi từng task về client
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int task_id = sqlite3_column_int(stmt, 0);
        const char *name = (const char *)sqlite3_column_text(stmt, 1);
        const char *description = (const char *)sqlite3_column_text(stmt, 2);
        const char *status = (const char *)sqlite3_column_text(stmt, 3);
        int assignee_id = sqlite3_column_int(stmt, 4);
        const char *due_date = (const char *)sqlite3_column_text(stmt, 5);

        snprintf(response, sizeof(response), 
                 "Task ID: %d, Name: %s, Description: %s, Status: %s, Assignee ID: %d, Due Date: %s\n", 
                 task_id, name, description, status, assignee_id, due_date ? due_date : "N/A");
        send_data(client_fd, response);
    }

    sqlite3_finalize(stmt);
    log_info("Sent task list for project %d to client_fd %d", project_id, client_fd);
}


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

void handle_data_message(int client_fd,int userid, const char* message) {
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

