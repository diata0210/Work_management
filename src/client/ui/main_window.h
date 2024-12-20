#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <gtk/gtk.h>

// Kích thước bộ đệm cho phản hồi server
#define BUFFER_SIZE 1024

// Hàm để phân tích phản hồi từ server và thêm project vào danh sách giao diện
void add_projects_to_list(GtkListBox *project_list, const char *response);

// Hàm tạo và trả về một cửa sổ chính hiển thị danh sách project
GtkWidget *create_main_window();

#endif /* MAIN_WINDOW_H */
