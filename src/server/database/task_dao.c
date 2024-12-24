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
TaskArray get_tasks_by_project(sqlite3 *db, int project_id) {
    const char *sql = "SELECT task_id, name, description, status, assignee_id, due_date FROM tasks WHERE project_id = ?";
    sqlite3_stmt *stmt;
    TaskArray result = {NULL, 0};

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to fetch tasks: %s\n", sqlite3_errmsg(db));
        return result;
    }

    sqlite3_bind_int(stmt, 1, project_id);

    // Đếm số lượng task
    int task_count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        task_count++;
    }

    // Quay lại đầu câu truy vấn
    sqlite3_reset(stmt);

    // Cấp phát bộ nhớ cho mảng task
    result.tasks = (Task *)malloc(task_count * sizeof(Task));
    if (result.tasks == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        sqlite3_finalize(stmt);
        return result;
    }
    result.count = task_count;

    // Lấy dữ liệu
    int index = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        result.tasks[index].task_id = sqlite3_column_int(stmt, 0);
        const char *name = (const char *)sqlite3_column_text(stmt, 1);
        const char *description = (const char *)sqlite3_column_text(stmt, 2);
        const char *status = (const char *)sqlite3_column_text(stmt, 3);
        result.tasks[index].assignee_id = sqlite3_column_int(stmt, 4);

        strncpy(result.tasks[index].name, name ? name : "", sizeof(result.tasks[index].name) - 1);
        result.tasks[index].name[sizeof(result.tasks[index].name) - 1] = '\0';

        strncpy(result.tasks[index].description, description ? description : "", sizeof(result.tasks[index].description) - 1);
        result.tasks[index].description[sizeof(result.tasks[index].description) - 1] = '\0';

        strncpy(result.tasks[index].status, status ? status : "", sizeof(result.tasks[index].status) - 1);
        result.tasks[index].status[sizeof(result.tasks[index].status) - 1] = '\0';

        
        index++;
    }

    sqlite3_finalize(stmt);
    return result;
}

void free_task_array(TaskArray *task_array) {
    if (task_array->tasks != NULL) {
        free(task_array->tasks);
        task_array->tasks = NULL;
        task_array->count = 0;
    }
}

CommentArray get_comments_by_task(sqlite3 *db, int task_id) {
    CommentArray result = {NULL, 0};
    const char *sql = "SELECT user_id, content FROM comments WHERE task_id = ?";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        log_error("Failed to prepare statement: %s", sqlite3_errmsg(db));
        return result;
    }

    sqlite3_bind_int(stmt, 1, task_id);

    Comment *comments = malloc(sizeof(Comment) * 100); // Giới hạn tối đa 100 comment, có thể tăng
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        comments[count].user_id = sqlite3_column_int(stmt, 0);
        strncpy(comments[count].content, (const char *)sqlite3_column_text(stmt, 1), sizeof(comments[count].content) - 1);
        count++;
    }

    result.comments = comments;
    result.count = count;
    sqlite3_finalize(stmt);
    return result;
}

// Hàm lấy file đính kèm từ cơ sở dữ liệu
AttachmentArray get_attachments_by_task(sqlite3 *db, int task_id) {
    AttachmentArray result = {NULL, 0};
    const char *sql = "SELECT filename, file_path, uploaded_by FROM attachments WHERE task_id = ?";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        log_error("Failed to prepare statement: %s", sqlite3_errmsg(db));
        return result;
    }

    sqlite3_bind_int(stmt, 1, task_id);

    Attachment *attachments = malloc(sizeof(Attachment) * 100); // Giới hạn tối đa 100 file, có thể tăng
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        strncpy(attachments[count].filename, (const char *)sqlite3_column_text(stmt, 0), sizeof(attachments[count].filename) - 1);
        strncpy(attachments[count].file_path, (const char *)sqlite3_column_text(stmt, 1), sizeof(attachments[count].file_path) - 1);
        attachments[count].uploaded_by = sqlite3_column_int(stmt, 2);
        count++;
    }

    result.attachments = attachments;
    result.count = count;
    sqlite3_finalize(stmt);
    return result;
}

// Hàm giải phóng bộ nhớ
void free_comment_array(CommentArray *array) {
    if (array->comments) {
        free(array->comments);
        array->comments = NULL;
    }
    array->count = 0;
}

void free_attachment_array(AttachmentArray *array) {
    if (array->attachments) {
        free(array->attachments);
        array->attachments = NULL;
    }
    array->count = 0;
}
