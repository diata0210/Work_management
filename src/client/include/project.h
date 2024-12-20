// src/client/include/project.h

#ifndef PROJECT_H
#define PROJECT_H

#include <gtk/gtk.h>
#include "client.h"

typedef struct {
    int id;
    char *name;
    char *description;
    char *created_date;
    int created_by;
    GList *members;
    GList *tasks;
} Project;

typedef struct {
    int id;
    char *name;
    char *description;
    char *deadline;
    char *status;
    int assigned_to;
    GList *comments;
    GList *attachments;
} Task;

// Tạo project mới
int project_create(Client *client, const char *name, const char *description);

// Thêm thành viên vào project
int project_add_member(Client *client, int project_id, int user_id, const char *role);

// Tạo task mới
int project_create_task(Client *client, int project_id, Task *task);

// Cập nhật thông tin task
int project_update_task(Client *client, Task *task);

// Lấy danh sách project
GList* project_get_list(Client *client);

// Lấy thông tin chi tiết project
Project* project_get_details(Client *client, int project_id);

// Giải phóng bộ nhớ project
void project_free(Project *project);

// Giải phóng bộ nhớ task
void task_free(Task *task);

#endif // PROJECT_H
