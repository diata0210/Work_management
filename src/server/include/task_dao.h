#ifndef TASK_DAO_H
#define TASK_DAO_H

#include <sqlite3.h>

int insert_task(sqlite3 *db, const char *name, const char *description, int project_id, int assignee_id);
int update_task_status(sqlite3 *db, int task_id, const char *status);
int add_comment_to_task(sqlite3 *db, int task_id, int user_id, const char *content);
int add_attachment_to_task(sqlite3 *db, int task_id, const char *filename, const char *file_path, int uploaded_by);
int get_tasks_by_project(sqlite3 *db, int project_id);

#endif 
