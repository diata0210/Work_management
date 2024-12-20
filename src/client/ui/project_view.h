#ifndef PROJECT_VIEW_H
#define PROJECT_VIEW_H

#include <gtk/gtk.h>
#include "../include/client.h"
#include "../include/project.h"

// Khai báo cấu trúc cho project_view
typedef struct {
    GtkWidget *window;          // Cửa sổ chính của project view
    GtkWidget *tasks_tree;      // Cây nhiệm vụ
    GtkWidget *members_list;    // Danh sách thành viên
    GtkWidget *chat_view;       // Khung chat
    GtkWidget *files_list;      // Danh sách tệp
} ProjectView;

// Các hàm liên quan đến project view
ProjectView* project_view_new(void);               // Tạo mới project view
void project_view_show(ProjectView *view);        // Hiển thị project view
void project_view_hide(ProjectView *view);        // Ẩn project view
void project_view_destroy(ProjectView *view);     // Hủy project view

#endif // PROJECT_VIEW_H
