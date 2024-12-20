#include <gtk/gtk.h>
#include "network/socket.h"
#include "ui/chat_window.h"
#include "task.h"

GtkWidget *create_project_window(int project_id);

void on_create_task_clicked(GtkButton *button, gpointer user_data);
void on_chat_button_clicked(GtkButton *button, gpointer user_data);
void load_tasks(GtkWidget *task_list, int project_id);

GtkWidget *create_project_window(int project_id) {
    // Tạo cửa sổ
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Project Tasks");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);

    // Tạo container chính
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Tạo danh sách task
    GtkWidget *task_list = gtk_list_box_new();
    gtk_box_pack_start(GTK_BOX(vbox), task_list, TRUE, TRUE, 0);

    // Tải danh sách task từ server
    load_tasks(task_list, project_id);

    // Tạo nút "Tạo Task"
    GtkWidget *create_task_button = gtk_button_new_with_label("Tạo Task");
    gtk_box_pack_start(GTK_BOX(vbox), create_task_button, FALSE, FALSE, 0);
    g_signal_connect(create_task_button, "clicked", G_CALLBACK(on_create_task_clicked), GINT_TO_POINTER(project_id));

    // Tạo nút "Chat"
    GtkWidget *chat_button = gtk_button_new_with_label("Chat");
    gtk_box_pack_start(GTK_BOX(vbox), chat_button, FALSE, FALSE, 0);
    g_signal_connect(chat_button, "clicked", G_CALLBACK(on_chat_button_clicked), GINT_TO_POINTER(project_id));

    // Hiển thị tất cả
    gtk_widget_show_all(window);
    return window;
}

void load_tasks(GtkWidget *task_list, int project_id) {
    char request[256];
    char response[1024];
    snprintf(request, sizeof(request), "CONTROL GET_TASKS %d", project_id);

    if (send_request(request, response)) {
        if (strncmp(response, "TASK_LIST_COUNT", 15) == 0) {
            int task_count;
            sscanf(response, "TASK_LIST_COUNT %d", &task_count);

            for (int i = 0; i < task_count; i++) {
                char task_info[256];
                receive_response(task_info, sizeof(task_info));
                
                GtkWidget *label = gtk_label_new(task_info);
                gtk_list_box_insert(GTK_LIST_BOX(task_list), label, -1);
            }
        } else {
            GtkWidget *error_label = gtk_label_new("Failed to load tasks.");
            gtk_list_box_insert(GTK_LIST_BOX(task_list), error_label, -1);
        }
    }
}

void on_create_task_clicked(GtkButton *button, gpointer user_data) {
    int project_id = GPOINTER_TO_INT(user_data);
    // Hiển thị form tạo task và gửi yêu cầu tới server để thêm task.
    g_print("Tạo task cho project_id: %d\n", project_id);
}

void on_chat_button_clicked(GtkButton *button, gpointer user_data) {
    int project_id = GPOINTER_TO_INT(user_data);
    GtkWidget *chat_window = create_chat_window(project_id);
    gtk_widget_show_all(chat_window);
}
