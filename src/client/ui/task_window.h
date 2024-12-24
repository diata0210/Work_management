#ifndef TASK_WINDOW_H
#define TASK_WINDOW_H

#include <gtk/gtk.h>

// Hàm tạo giao diện chi tiết task
GtkWidget *create_task_window(const char *task_name, const char *description, const char *status, int assignee_id, int task_id);

// Callback cho nút cập nhật tiến độ
void on_update_progress_clicked(GtkButton *button, gpointer user_data);

// Callback cho nút thêm comment
void on_add_comment_clicked(GtkButton *button, gpointer user_data);

// Callback cho nút thêm file đính kèm
void on_add_attachment_clicked(GtkButton *button, gpointer user_data);

#endif // TASK_WINDOW_H
