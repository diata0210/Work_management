#include <gtk/gtk.h>
#include "main_window.h"
#include "../network/socket.h"
#include "../message_handlers/control_message_handler.h"

#define BUFFER_SIZE 1024

// Hàm để phân tích và thêm project vào giao diện
void add_projects_to_list(GtkListBox *project_list, const char *response) {
    char *line = strtok((char *)response, "\n");
    while (line != NULL) {
        if (strncmp(line, "PROJECT_ID", 10) == 0) {
            // Phân tích thông tin project
            int project_id;
            char project_name[256];
            sscanf(line, "PROJECT_ID: %d, NAME: %[^\n]", &project_id, project_name);

            // Thêm project vào danh sách
            GtkWidget *row = gtk_label_new(project_name);
            gtk_list_box_insert(project_list, row, -1);
        }
        line = strtok(NULL, "\n");
    }
}

// Hàm tạo màn hình chính hiển thị danh sách project
GtkWidget *create_main_window() {
    // Tạo cửa sổ chính
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Project Management");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    // Tạo một container chính
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Tiêu đề
    GtkWidget *title_label = gtk_label_new("Your Projects");
    gtk_widget_set_halign(title_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(vbox), title_label, FALSE, FALSE, 5);

    // Tạo danh sách project
    GtkWidget *project_list = gtk_list_box_new();
    gtk_box_pack_start(GTK_BOX(vbox), project_list, TRUE, TRUE, 5);

    // Gửi yêu cầu GET_PROJECT và nhận phản hồi
    char response[BUFFER_SIZE];
    if (send_get_projects_request(response, BUFFER_SIZE)) {
        if (strncmp(response, "NO_PROJECTS", 11) == 0) {
            GtkWidget *no_projects_label = gtk_label_new("No projects available.");
            gtk_box_pack_start(GTK_BOX(vbox), no_projects_label, FALSE, FALSE, 5);
        } else {
            add_projects_to_list(GTK_LIST_BOX(project_list), response);
        }
    } else {
        GtkWidget *error_label = gtk_label_new("Failed to load projects.");
        gtk_box_pack_start(GTK_BOX(vbox), error_label, FALSE, FALSE, 5);
    }

    // Nút Logout
    GtkWidget *logout_button = gtk_button_new_with_label("Logout");
    gtk_widget_set_halign(logout_button, GTK_ALIGN_END);
    gtk_box_pack_start(GTK_BOX(vbox), logout_button, FALSE, FALSE, 5);

    // Gắn callback cho nút Logout
    g_signal_connect(logout_button, "clicked", G_CALLBACK(gtk_main_quit), NULL);

    // Hiển thị tất cả các widget
    gtk_widget_show_all(window);

    return window;
}
