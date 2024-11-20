#include <stdio.h>
#include <sqlite3.h>
#include "task_dao.h"

// Thêm công việc mới vào dự án và gán cho thành viên
int insert_task(sqlite3 *db, const char *name, const char *description, int project_id, int assignee_id) {
    char *err_msg = 0;
    char sql[256];
    snprintf(sql, sizeof(sql), "INSERT INTO tasks (name, description, project_id, assignee_id, status) VALUES ('%s', '%s', %d, %d, 'not started');", 
             name, description, project_id, assignee_id);

    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return rc;
    }

    printf("Task added successfully\n");
    return SQLITE_OK;
}

// Cập nhật trạng thái của công việc
int update_task_status(sqlite3 *db, int task_id, const char *status) {
    char sql[256];
    char *err_msg = 0;
    
    snprintf(sql, sizeof(sql), "UPDATE tasks SET status = '%s' WHERE task_id = %d;", status, task_id);

    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return rc;
    }

    printf("Task status updated successfully\n");
    return SQLITE_OK;
}

// Thêm nhận xét vào công việc
int add_comment_to_task(sqlite3 *db, int task_id, int user_id, const char *content) {
    char sql[256];
    char *err_msg = 0;

    snprintf(sql, sizeof(sql), "INSERT INTO comments (task_id, user_id, content) VALUES (%d, %d, '%s');", task_id, user_id, content);

    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return rc;
    }

    printf("Comment added to task successfully\n");
    return SQLITE_OK;
}

// Thêm file đính kèm vào công việc
int add_attachment_to_task(sqlite3 *db, int task_id, const char *filename, const char *file_path, int uploaded_by) {
    char sql[256];
    char *err_msg = 0;

    snprintf(sql, sizeof(sql), "INSERT INTO attachments (task_id, filename, file_path, uploaded_by) VALUES (%d, '%s', '%s', %d);", 
             task_id, filename, file_path, uploaded_by);

    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return rc;
    }

    printf("Attachment added to task successfully\n");
    return SQLITE_OK;
}

// Lấy danh sách công việc thuộc một dự án
int get_tasks_by_project(sqlite3 *db, int project_id) {
    char sql[256];
    sqlite3_stmt *stmt;

    snprintf(sql, sizeof(sql), "SELECT task_id, name, description, status, assignee_id, due_date FROM tasks WHERE project_id = %d;", project_id);

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to fetch tasks: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    printf("Tasks for Project ID %d:\n", project_id);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int task_id = sqlite3_column_int(stmt, 0);
        const char *name = (const char *)sqlite3_column_text(stmt, 1);
        const char *description = (const char *)sqlite3_column_text(stmt, 2);
        const char *status = (const char *)sqlite3_column_text(stmt, 3);
        int assignee_id = sqlite3_column_int(stmt, 4);
        const char *due_date = (const char *)sqlite3_column_text(stmt, 5);

        printf("Task ID: %d, Name: %s, Description: %s, Status: %s, Assignee ID: %d, Due Date: %s\n", 
               task_id, name, description, status, assignee_id, due_date ? due_date : "N/A");
    }

    sqlite3_finalize(stmt);
    return SQLITE_OK;
}
