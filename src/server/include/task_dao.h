#ifndef TASK_DAO_H
#define TASK_DAO_H

#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>

// Cấu trúc đại diện cho một công việc
typedef struct {
    int task_id;
    char name[128];
    char description[256];
    char status[32];
    int assignee_id;
    char due_date[20];
} Task;

// Cấu trúc đại diện cho mảng công việc
typedef struct {
    Task *tasks;
    int count;
} TaskArray;

typedef struct {
    int user_id;
    char content[256];
} Comment;

typedef struct {
    Comment *comments;
    int count;
} CommentArray;

typedef struct {
    char filename[128];
    char file_path[256];
    int uploaded_by;
} Attachment;

typedef struct {
    Attachment *attachments;
    int count;
} AttachmentArray;

// Khai báo các hàm
int insert_task(sqlite3 *db, const char *name, const char *description, int project_id, int assignee_id);
int update_task_status(sqlite3 *db, int task_id, const char *status);
int add_comment_to_task(sqlite3 *db, int task_id, int user_id, const char *content);
int add_attachment_to_task(sqlite3 *db, int task_id, const char *filename, const char *file_path, int uploaded_by);
TaskArray get_tasks_by_project(sqlite3 *db, int project_id);
void free_task_array(TaskArray *task_array);


CommentArray get_comments_by_task(sqlite3 *db, int task_id);
AttachmentArray get_attachments_by_task(sqlite3 *db, int task_id);
void free_comment_array(CommentArray *array) ;
void free_attachment_array(AttachmentArray *array) ;



#endif // TASK_DAO_H
