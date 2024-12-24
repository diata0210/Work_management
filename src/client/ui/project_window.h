// #ifndef PROJECT_WINDOW_H
// #define PROJECT_WINDOW_H

// #include <gtk/gtk.h>

// typedef struct {
//     int task_id;
//     char task_name[256];
//     char description[512];
//     char status[50];
//     int assignee_id;
// } Task;

// // Cấu trúc lưu danh sách Task
// typedef struct {
//     Task tasks[100]; // Tối đa 100 task
//     int count;
// } TaskList;

// // Khai báo các hàm giao diện
// GtkWidget *create_project_window(int project_id);
// GtkWidget *create_gantt_chart(TaskList *task_list_data);

// // Hàm thao tác với danh sách Task
// void load_tasks(GtkWidget *task_list, int project_id, TaskList *task_list_data);
// void refresh_project_window(GtkWidget *project_window, int project_id);

// // Callback functions
// void on_task_clicked(GtkButton *button, gpointer user_data);
// void on_create_task_clicked(GtkButton *button, gpointer user_data);
// void on_chat_button_clicked(GtkButton *button, gpointer user_data);


// #endif // PROJECT_WINDOW_H
#ifndef PROJECT_WINDOW_H
#define PROJECT_WINDOW_H

#include <gtk/gtk.h>

// Định nghĩa cấu trúc Task
typedef struct {
    int task_id;
    char task_name[256];
    char description[512];
    char status[50];
    int assignee_id;
} Task;

// Định nghĩa cấu trúc TaskList
typedef struct {
    Task tasks[100];
    int count;
} TaskList;
typedef struct {
    int project_id;
    GtkWidget *project_window;
} ProjectContext;

// Hàm tạo phần hiển thị kết hợp danh sách Task và Gantt Chart
GtkWidget *create_combined_task_view(TaskList *task_list_data);

// Hàm callback xử lý sự kiện nhấn vào nút "Tạo Task"
void on_create_task_clicked(GtkButton *button, gpointer user_data);
void load_members(GtkWidget *list_box, int project_id);
// Hàm callback xử lý sự kiện nhấn vào nút "Chat"
void on_chat_button_clicked(GtkButton *button, gpointer user_data);

// Hàm callback xử lý sự kiện nhấn vào một Task
void on_task_clicked(GtkButton *button, gpointer user_data);

// Làm mới cửa sổ dự án sau khi có thay đổi
void refresh_project_window(GtkWidget *project_window, ProjectContext *context);
// Tải danh sách Task từ server vào giao diện
void load_tasks(GtkWidget *task_list, int project_id, TaskList *task_list_data);

// Hàm tạo cửa sổ giao diện dự án
GtkWidget *create_project_window(int project_id);

#endif // PROJECT_WINDOW_H

