#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <gtk/gtk.h>

// Biến toàn cục đại diện cho cửa sổ chính
extern GtkWidget *main_window;

// Hàm để phân tích và thêm project vào giao diện
void add_projects_to_list(GtkListBox *project_list, const char *response);

// Hàm callback khi nhấn nút Tạo Project

// Hàm callback khi hoàn tất tạo project

// Hàm tạo màn hình chính hiển thị danh sách project
GtkWidget *create_main_window();

// Hàm tạo cửa sổ "Create Project"
GtkWidget *create_create_project_window();

#endif // MAIN_WINDOW_H
