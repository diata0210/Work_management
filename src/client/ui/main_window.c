#include <gtk/gtk.h>
#include "main_window.h"

#include "../network/socket.h"
#include "../message_handlers/control_message_handler.h"

#define BUFFER_SIZE 4096

GtkWidget *main_window;

// Hàm để phân tích và thêm project vào giao diện
#include <glib.h> // Để sử dụng các hàm liên quan đến UTF-8

static void on_project_clicked(GtkButton *button, gpointer user_data) {
    int project_id = GPOINTER_TO_INT(user_data);
    printf("%d",project_id);
    GtkWidget *project_window = create_project_window(project_id);
    gtk_widget_show_all(project_window);
}
void add_projects_to_list(GtkListBox *project_list, const char *response) {
    const char *start = response;
    const char *end;

    while ((end = strchr(start, '\n')) != NULL) {
        // Tách từng dòng
        size_t length = end - start;
        char line[BUFFER_SIZE];
        strncpy(line, start, length);
        line[length] = '\0';

        printf("Line: %s\n", line);

        // Kiểm tra và xử lý nếu là dòng chứa thông tin project
        if (strncmp(line, "PROJECT_ID", 10) == 0) {
            int project_id;
            char project_name[256];
            sscanf(line, "PROJECT_ID: %d, NAME: %255[^\n]", &project_id, project_name);

            // Kiểm tra và đảm bảo chuỗi project_name là UTF-8
            if (!g_utf8_validate(project_name, -1, NULL)) {
                fprintf(stderr, "Invalid UTF-8 string: %s\n", project_name);
                continue;
            }

            // Thêm project vào danh sách
            GtkWidget *button = gtk_button_new_with_label(project_name);
            gtk_list_box_insert(project_list, button, -1);

            // Gắn callback khi nhấn vào project
            g_signal_connect(button, "clicked", G_CALLBACK(on_project_clicked), GINT_TO_POINTER(project_id));
        }

        // Chuyển sang dòng tiếp theo
        start = end + 1;
    }

    // Xử lý dòng cuối cùng nếu không có dấu '\n'
    if (*start != '\0') {
        printf("Line: %s\n", start);

        if (strncmp(start, "PROJECT_ID", 10) == 0) {
            int project_id;
            char project_name[256];
            sscanf(start, "PROJECT_ID: %d, NAME: %255[^\n]", &project_id, project_name);

            if (!g_utf8_validate(project_name, -1, NULL)) {
                fprintf(stderr, "Invalid UTF-8 string: %s\n", project_name);
                return;
            }

            GtkWidget *button = gtk_button_new_with_label(project_name);
            gtk_list_box_insert(project_list, button, -1);
            g_signal_connect(button, "clicked", G_CALLBACK(on_project_clicked), GINT_TO_POINTER(project_id));
        }
    }
}


// Hàm callback khi nhấn nút Tạo Project
static void on_create_project_button_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *create_project_window = create_create_project_window();
    gtk_widget_show_all(create_project_window);
}

// Hàm callback khi hoàn tất tạo project
static void on_project_created(GtkButton *button, gpointer user_data) {
    // Lấy cửa sổ tạo project
    GtkWidget *create_project_window = GTK_WIDGET(user_data);

    // Lấy các widget từ dữ liệu của cửa sổ tạo project
    GtkWidget *name_entry = GTK_WIDGET(g_object_get_data(G_OBJECT(create_project_window), "name_entry"));
    GtkWidget *description_entry = GTK_WIDGET(g_object_get_data(G_OBJECT(create_project_window), "description_entry"));

    // Kiểm tra xem các widget có hợp lệ hay không
    if (!GTK_IS_ENTRY(name_entry) || !GTK_IS_ENTRY(description_entry)) {
        fprintf(stderr, "Error: name_entry or description_entry is not a valid GtkEntry.\n");
        return;
    }

    // Lấy dữ liệu từ các trường nhập liệu
    const char *project_name = gtk_entry_get_text(GTK_ENTRY(name_entry));
    const char *description = gtk_entry_get_text(GTK_ENTRY(description_entry));

    if (project_name == NULL || description == NULL || strlen(project_name) == 0) {
        fprintf(stderr, "Error: Project name or description is empty.\n");
        return;
    }

    // Gọi hàm send_create_project
    if (send_create_project(project_name, description)) {
        gtk_widget_destroy(create_project_window);

        // Làm mới giao diện màn hình chính
        gtk_widget_destroy(main_window);
        main_window = create_main_window();
        gtk_widget_show_all(main_window);
    } else {
        fprintf(stderr, "Failed to create project.\n");
    }
}

// Hàm tạo cửa sổ tạo project
GtkWidget *create_create_project_window() {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Create Project");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    GtkWidget *name_label = gtk_label_new("Name:");
    GtkWidget *name_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(name_entry), "Enter project name");

    GtkWidget *description_label = gtk_label_new("Description:");
    GtkWidget *description_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(description_entry), "Enter project description");

    GtkWidget *create_button = gtk_button_new_with_label("Create");
    g_signal_connect(create_button, "clicked", G_CALLBACK(on_project_created), window);

    // Lưu các widget cần thiết vào dữ liệu của cửa sổ
    g_object_set_data(G_OBJECT(window), "name_entry", name_entry);
    g_object_set_data(G_OBJECT(window), "description_entry", description_entry);

    gtk_grid_attach(GTK_GRID(grid), name_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), name_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), description_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), description_entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), create_button, 0, 2, 2, 1);

    return window;
}

// Hàm tạo màn hình chính hiển thị danh sách project
GtkWidget *create_main_window() {
    // Tạo cửa sổ chính
    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window), "Project Management");
    gtk_window_set_default_size(GTK_WINDOW(main_window), 800, 600);

    // Tạo một container chính
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
    gtk_container_add(GTK_CONTAINER(main_window), vbox);

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
            printf("%s",response);
            printf("1");
            
            add_projects_to_list(GTK_LIST_BOX(project_list), response);
        }
    } else {
        GtkWidget *error_label = gtk_label_new("Failed to load projects.");
        gtk_box_pack_start(GTK_BOX(vbox), error_label, FALSE, FALSE, 5);
    }

    // Nút Tạo Project
    GtkWidget *create_project_button = gtk_button_new_with_label("Create Project");
    gtk_box_pack_start(GTK_BOX(vbox), create_project_button, FALSE, FALSE, 5);
    g_signal_connect(create_project_button, "clicked", G_CALLBACK(on_create_project_button_clicked), NULL);

    // Nút Logout
    GtkWidget *logout_button = gtk_button_new_with_label("Logout");
    gtk_widget_set_halign(logout_button, GTK_ALIGN_END);
    gtk_box_pack_start(GTK_BOX(vbox), logout_button, FALSE, FALSE, 5);

    // Gắn callback cho nút Logout
    g_signal_connect(logout_button, "clicked", G_CALLBACK(gtk_main_quit), NULL);

    // Hiển thị tất cả các widget
    gtk_widget_show_all(main_window);

    return main_window;
}
