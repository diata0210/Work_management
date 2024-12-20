#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <gtk/gtk.h>

// Cấu trúc chứa các widget của cửa sổ chính
typedef struct {
    GtkWidget *window;           // Cửa sổ chính
    GtkWidget *projects_tree;    // Cây dự án
    GtkWidget *tasks_tree;       // Cây công việc
    GtkWidget *chat_window;      // Cửa sổ chat
} MainWindow;

// Khai báo các hàm để khởi tạo, hiển thị, và hủy cửa sổ chính
void main_window_init(MainWindow *main_window); // Khởi tạo cửa sổ chính
void main_window_show(MainWindow *main_window); // Hiển thị cửa sổ chính
void main_window_destroy(MainWindow *main_window); // Hủy cửa sổ chính

#endif // MAIN_WINDOW_H
