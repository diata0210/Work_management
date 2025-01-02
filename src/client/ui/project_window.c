#include <gtk/gtk.h>
#include "../network/socket.h"
#include "../message_handlers/data_message_handler.h"
#include "../message_handlers/control_message_handler.h"
#include "project_window.h"
#include "chat_window.h"
#include "task_window.h"

// Định nghĩa cấu trúc để lưu thông tin project



// // #include "task.h"


// GtkWidget *create_combined_task_view(TaskList *task_list_data) {
//     GtkWidget *scroll_window = gtk_scrolled_window_new(NULL, NULL);
//     gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

//     GtkWidget *list_box = gtk_list_box_new();
//     gtk_container_add(GTK_CONTAINER(scroll_window), list_box);

//     for (int i = 0; i < task_list_data->count; i++) {
//         Task *task = &task_list_data->tasks[i];

//         // Tạo hàng hiển thị task
//         GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

//         // Label tên task
//         GtkWidget *task_label = gtk_label_new(task->task_name);
//         gtk_box_pack_start(GTK_BOX(row), task_label, FALSE, FALSE, 0);

//         // Mô tả task
//         GtkWidget *description_label = gtk_label_new(task->description);
//         gtk_box_pack_start(GTK_BOX(row), description_label, TRUE, TRUE, 0);

//         // Thanh tiến độ
//         GtkWidget *progress_bar = gtk_progress_bar_new();
//         if (strcmp(task->status, "not started") == 0) {
//             gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), 0.0);
//             gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress_bar), "Not Started");
//         } else if (strcmp(task->status, "inprogress") == 0) {
//             gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), 0.5);
//             gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress_bar), "In Progress");
//         } else if (strcmp(task->status, "completed") == 0) {
//             gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), 1.0);
//             gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress_bar), "Completed");
//         }
//         gtk_box_pack_start(GTK_BOX(row), progress_bar, TRUE, TRUE, 0);

//         // Tạo nút chi tiết task
//         GtkWidget *button = gtk_button_new_with_label("View Details");
//         char *user_data = g_strdup_printf("%s;%s;%s;%d;%d", task->task_name, task->description, task->status, task->assignee_id, task->task_id);
//         g_signal_connect(button, "clicked", G_CALLBACK(on_task_clicked), user_data);
//         gtk_box_pack_start(GTK_BOX(row), button, FALSE, FALSE, 0);

//         gtk_list_box_insert(GTK_LIST_BOX(list_box), row, -1);
//     }

//     return scroll_window;
// }




// void on_create_task_clicked(GtkButton *button, gpointer user_data);
// void on_chat_button_clicked(GtkButton *button, gpointer user_data);
void on_task_clicked(GtkButton *button, gpointer user_data) {
    if (user_data == NULL) {
        fprintf(stderr, "Error: user_data is NULL. Cannot process task.\n");
        return;
    }

    char *data = (char *)user_data;
    char task_name[256], description[512], status[50];
    int assignee_id, task_id;

    printf("Received user data: %s\n", data);

    if (sscanf(data, "%255[^;];%511[^;];%49[^;];%d;%d", task_name, description, status, &assignee_id, &task_id) != 5) {
        fprintf(stderr, "Error: Failed to parse user_data. Data: %s\n", data);
        return;
    }

    // In thông tin task để debug
    printf("Task Information:\n");
    printf("  - Task Name: %s\n", task_name);
    printf("  - Description: %s\n", description);
    printf("  - Status: %s\n", status);
    printf("  - Assignee ID: %d\n", assignee_id);
    printf("  - Task ID: %d\n", task_id);

    // Tạo cửa sổ chi tiết task
    GtkWidget *task_window = create_task_window(task_name, description, status, assignee_id, task_id);
    if (task_window) {
        gtk_widget_show_all(task_window);
    } else {
        fprintf(stderr, "Failed to create task window for Task ID: %d\n", task_id);
    }

    // Giải phóng dữ liệu sau khi sử dụng
    g_free(user_data);
}

bool load_tasks_from_server(int project_id, TaskList *task_list_data) {
    char response[4096];
    task_list_data->count = 0;

    // Gửi yêu cầu lấy danh sách task
    if (!send_get_tasks_request(project_id, response, sizeof(response))) {
        return false;
    }

    const char *start = response;
    const char *end;

    if (strncmp(response, "NO_TASKS", 8) == 0) {
        return true; // Không có task nào nhưng không có lỗi
    }

    while ((end = strchr(start, '\n')) != NULL) {
        size_t length = end - start;
        char line[256];
        strncpy(line, start, length);
        line[length] = '\0';

        if (strncmp(line, "TASK_ID", 7) == 0) {
            int task_id, assignee_id;
            char task_name[256], task_description[512], task_status[50];

            if (sscanf(line,
                       "TASK_ID: %d, NAME: %255[^,], DESCRIPTION: %511[^,], STATUS: %49[^,], ASSIGNEE_ID: %d",
                       &task_id, task_name, task_description, task_status, &assignee_id) == 5) {
                if (task_list_data->count < 100) {
                    Task *task = &task_list_data->tasks[task_list_data->count++];
                    task->task_id = task_id;
                    strncpy(task->task_name, task_name, sizeof(task->task_name) - 1);
                    strncpy(task->description, task_description, sizeof(task->description) - 1);
                    strncpy(task->status, task_status, sizeof(task->status) - 1);
                    task->assignee_id = assignee_id;
                }
            }
        }

        start = end + 1;
    }

    return true;
}

// void refresh_project_window(GtkWidget *project_window, int project_id) {
//     // Lấy container chính của project_window
//     GtkWidget *vbox = gtk_bin_get_child(GTK_BIN(project_window));

//     // Xóa toàn bộ nội dung hiện tại
//     GList *children = gtk_container_get_children(GTK_CONTAINER(vbox));
//     for (GList *child = children; child != NULL; child = g_list_next(child)) {
//         gtk_widget_destroy(GTK_WIDGET(child->data));
//     }
//     g_list_free(children);

//     // Tạo lại nội dung giao diện
//     TaskList task_list_data = {0};
//     if (!load_tasks_from_server(project_id, &task_list_data)) {
//         GtkWidget *error_label = gtk_label_new("Failed to load tasks.");
//         gtk_box_pack_start(GTK_BOX(vbox), error_label, TRUE, TRUE, 0);
//     } else {
//         // Tạo phần hiển thị danh sách Task và Gantt Chart
//         GtkWidget *combined_view = create_combined_task_view(&task_list_data);
//         gtk_box_pack_start(GTK_BOX(vbox), combined_view, TRUE, TRUE, 0);
//     }

//     // Tạo nút "Tạo Task"
//     GtkWidget *create_task_button = gtk_button_new_with_label("Tạo Task");
//     gtk_box_pack_start(GTK_BOX(vbox), create_task_button, FALSE, FALSE, 0);
//     g_signal_connect(create_task_button, "clicked", G_CALLBACK(on_create_task_clicked), GINT_TO_POINTER(project_id));

//     // Tạo nút "Chat"
//     GtkWidget *chat_button = gtk_button_new_with_label("Chat");
//     gtk_box_pack_start(GTK_BOX(vbox), chat_button, FALSE, FALSE, 0);
//     g_signal_connect(chat_button, "clicked", G_CALLBACK(on_chat_button_clicked), GINT_TO_POINTER(project_id));

//     // Hiển thị lại toàn bộ nội dung
//     gtk_widget_show_all(project_window);
// }




// void on_create_task_clicked(GtkButton *button, gpointer user_data) {
//     int project_id = GPOINTER_TO_INT(user_data);
//     printf("%d",project_id);
//     // Hiển thị hộp thoại nhập thông tin task
//     GtkWidget *dialog = gtk_dialog_new_with_buttons("Tạo Task",
//                                                     NULL,
//                                                     GTK_DIALOG_MODAL,
//                                                     "Tạo", GTK_RESPONSE_ACCEPT,
//                                                     "Hủy", GTK_RESPONSE_CANCEL,
//                                                     NULL);

//     GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

//     GtkWidget *grid = gtk_grid_new();
//     gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
//     gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
//     gtk_container_add(GTK_CONTAINER(content_area), grid);

//     // Trường nhập tên task
//     GtkWidget *task_name_label = gtk_label_new("Tên Task:");
//     GtkWidget *task_name_entry = gtk_entry_new();
//     gtk_grid_attach(GTK_GRID(grid), task_name_label, 0, 0, 1, 1);
//     gtk_grid_attach(GTK_GRID(grid), task_name_entry, 1, 0, 1, 1);

//     // Trường nhập mô tả task
//     GtkWidget *description_label = gtk_label_new("Mô tả:");
//     GtkWidget *description_entry = gtk_entry_new();
//     gtk_grid_attach(GTK_GRID(grid), description_label, 0, 1, 1, 1);
//     gtk_grid_attach(GTK_GRID(grid), description_entry, 1, 1, 1, 1);

//     // Trường nhập ID người nhận task
//     GtkWidget *assignee_label = gtk_label_new("Assignee ID:");
//     GtkWidget *assignee_entry = gtk_entry_new();
//     gtk_grid_attach(GTK_GRID(grid), assignee_label, 0, 2, 1, 1);
//     gtk_grid_attach(GTK_GRID(grid), assignee_entry, 1, 2, 1, 1);

//     gtk_widget_show_all(dialog);

//     // Xử lý phản hồi từ hộp thoại
//     if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
//         const char *task_name = gtk_entry_get_text(GTK_ENTRY(task_name_entry));
//         const char *description = gtk_entry_get_text(GTK_ENTRY(description_entry));
//         const char *assignee_id_text = gtk_entry_get_text(GTK_ENTRY(assignee_entry));
//         int assignee_id = atoi(assignee_id_text);

//         // Gửi yêu cầu tạo task
//         if (send_create_task(task_name, description, project_id, assignee_id)) {
//             g_print("Task '%s' đã được tạo thành công.\n", task_name);
//             GtkWidget *task_list = gtk_list_box_new(); // Tìm widget task_list và truyền đúng object vào đây
//             // load_tasks(task_list, project_id);
//             refresh_project_window(project_window, project_id); // Làm mới giao diện

//         } else {
//             g_print("Không thể tạo task '%s'.\n", task_name);
//         }
//     }
//     gtk_widget_destroy(dialog);
// }


// // void on_create_task_clicked(GtkButton *button, gpointer user_data) {
// //     struct {
// //         GtkWidget *project_window;
// //         int project_id;
// //     } *data = user_data;

// //     int project_id = data->project_id;
// //     GtkWidget *project_window = data->project_window;

// //     // Hiển thị hộp thoại nhập thông tin task
// //     GtkWidget *dialog = gtk_dialog_new_with_buttons("Tạo Task",
// //                                                     GTK_WINDOW(project_window),
// //                                                     GTK_DIALOG_MODAL,
// //                                                     "Tạo", GTK_RESPONSE_ACCEPT,
// //                                                     "Hủy", GTK_RESPONSE_CANCEL,
// //                                                     NULL);

// //     GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
// //     GtkWidget *grid = gtk_grid_new();
// //     gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
// //     gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
// //     gtk_container_add(GTK_CONTAINER(content_area), grid);

// //     // Trường nhập tên task
// //     GtkWidget *task_name_label = gtk_label_new("Tên Task:");
// //     GtkWidget *task_name_entry = gtk_entry_new();
// //     gtk_grid_attach(GTK_GRID(grid), task_name_label, 0, 0, 1, 1);
// //     gtk_grid_attach(GTK_GRID(grid), task_name_entry, 1, 0, 1, 1);

// //     // Trường nhập mô tả task
// //     GtkWidget *description_label = gtk_label_new("Mô tả:");
// //     GtkWidget *description_entry = gtk_entry_new();
// //     gtk_grid_attach(GTK_GRID(grid), description_label, 0, 1, 1, 1);
// //     gtk_grid_attach(GTK_GRID(grid), description_entry, 1, 1, 1, 1);

// //     // Trường nhập ID người nhận task
// //     GtkWidget *assignee_label = gtk_label_new("Assignee ID:");
// //     GtkWidget *assignee_entry = gtk_entry_new();
// //     gtk_grid_attach(GTK_GRID(grid), assignee_label, 0, 2, 1, 1);
// //     gtk_grid_attach(GTK_GRID(grid), assignee_entry, 1, 2, 1, 1);

// //     gtk_widget_show_all(dialog);

// //     // Xử lý phản hồi từ hộp thoại
// //     if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
// //         const char *task_name = gtk_entry_get_text(GTK_ENTRY(task_name_entry));
// //         const char *description = gtk_entry_get_text(GTK_ENTRY(description_entry));
// //         const char *assignee_id_text = gtk_entry_get_text(GTK_ENTRY(assignee_entry));
// //         int assignee_id = atoi(assignee_id_text);

// //         // Gửi yêu cầu tạo task
// //         if (send_create_task(task_name, description, project_id, assignee_id)) {
// //             g_print("Task '%s' đã được tạo thành công.\n", task_name);

// //             // Làm mới màn hình project_window
// //             refresh_project_window(project_window, project_id);
// //         } else {
// //             g_print("Không thể tạo task '%s'.\n", task_name);
// //         }
// //     }

// //     gtk_widget_destroy(dialog);
// // }
void on_chat_button_clicked(GtkButton *button, gpointer user_data) {
    ProjectContext *context = (ProjectContext*)user_data;
    int project_id = context->project_id;
    
    GtkWidget *chat_window = create_chat_window(project_id);
    gtk_widget_show_all(chat_window);
}
// void load_tasks(GtkWidget *task_list, int project_id, TaskList *task_list_data) {
//     char response[4096]; // Bộ đệm cho phản hồi
//     task_list_data->count = 0;

//     // Gửi yêu cầu lấy danh sách tasks
//     if (!send_get_tasks_request(project_id, response, sizeof(response))) {
//         GtkWidget *error_label = gtk_label_new("Failed to load tasks.");
//         gtk_list_box_insert(GTK_LIST_BOX(task_list), error_label, -1);
//         return;
//     }

//     const char *start = response;
//     const char *end;

//     // Kiểm tra phản hồi từ server
//     if (strncmp(response, "NO_TASKS", 8) == 0) {
//         GtkWidget *no_tasks_label = gtk_label_new("No tasks available for this project.");
//         gtk_list_box_insert(GTK_LIST_BOX(task_list), no_tasks_label, -1);
//         return;
//     }

//     // Duyệt qua từng dòng của phản hồi
//     while ((end = strchr(start, '\n')) != NULL) {
//         size_t length = end - start;
//         char line[256];
//         strncpy(line, start, length);
//         line[length] = '\0';

//         if (strncmp(line, "TASK_ID", 7) == 0) {
//             int task_id, assignee_id;
//             char task_name[256], task_description[512], task_status[50];

//             if (sscanf(line,
//                        "TASK_ID: %d, NAME: %255[^,], DESCRIPTION: %511[^,], STATUS: %49[^,], ASSIGNEE_ID: %d",
//                        &task_id, task_name, task_description, task_status, &assignee_id) == 5) {
//                 // Lưu thông tin task vào cấu trúc
//                 if (task_list_data->count < 100) {
//                     Task *task = &task_list_data->tasks[task_list_data->count++];
//                     task->task_id = task_id;
//                     strncpy(task->task_name, task_name, sizeof(task->task_name) - 1);
//                     strncpy(task->description, task_description, sizeof(task->description) - 1);
//                     strncpy(task->status, task_status, sizeof(task->status) - 1);
//                     task->assignee_id = assignee_id;
//                 }

//                 // Tạo nút hiển thị task
//                 char display_text[512];
//                 snprintf(display_text, sizeof(display_text), "%s - %s (%s)", task_name, task_description, task_status);

//                 GtkWidget *button = gtk_button_new_with_label(display_text);
//                 gtk_list_box_insert(GTK_LIST_BOX(task_list), button, -1);

//                 // Gắn callback mở chi tiết task
//                 char *user_data = g_strdup_printf("%s;%s;%s;%d;%d", task_name, task_description, task_status, assignee_id, task_id);
//                 g_signal_connect(button, "clicked", G_CALLBACK(on_task_clicked), user_data);
//             }
//         }
//         start = end + 1;
//     }
// }








// GtkWidget *create_project_window(int project_id) {
//     GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//     gtk_window_set_title(GTK_WINDOW(window), "Project Tasks and Gantt Chart");
//     gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

//     GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
//     gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
//     gtk_container_add(GTK_CONTAINER(window), vbox);

//     // Tải danh sách task từ server
//     TaskList task_list_data = {0};
//     if (!load_tasks_from_server(project_id, &task_list_data)) {
//         GtkWidget *error_label = gtk_label_new("Failed to load tasks.");
//         gtk_box_pack_start(GTK_BOX(vbox), error_label, TRUE, TRUE, 0);
//         gtk_widget_show_all(window);
//         return window;
//     }

//     // Tạo phần hiển thị kết hợp
//     GtkWidget *combined_view = create_combined_task_view(&task_list_data);
//     gtk_box_pack_start(GTK_BOX(vbox), combined_view, TRUE, TRUE, 0);

//     // Tạo nút "Tạo Task"
//     GtkWidget *create_task_button = gtk_button_new_with_label("Tạo Task");
//     gtk_box_pack_start(GTK_BOX(vbox), create_task_button, FALSE, FALSE, 0);
//     g_signal_connect(create_task_button, "clicked", G_CALLBACK(on_create_task_clicked), GINT_TO_POINTER(project_id));

//     // Tạo nút "Chat"
//     GtkWidget *chat_button = gtk_button_new_with_label("Chat");
//     gtk_box_pack_start(GTK_BOX(vbox), chat_button, FALSE, FALSE, 0);
//     g_signal_connect(chat_button, "clicked", G_CALLBACK(on_chat_button_clicked), GINT_TO_POINTER(project_id));

//     gtk_widget_show_all(window);
//     return window;
// }








// Hàm tạo danh sách task hiển thị
GtkWidget *create_combined_task_view(TaskList *task_list_data) {
    GtkWidget *scroll_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    GtkWidget *list_box = gtk_list_box_new();
    gtk_container_add(GTK_CONTAINER(scroll_window), list_box);

    for (int i = 0; i < task_list_data->count; i++) {
        Task *task = &task_list_data->tasks[i];

        GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

        GtkWidget *task_label = gtk_label_new(task->task_name);
        gtk_box_pack_start(GTK_BOX(row), task_label, FALSE, FALSE, 0);

        GtkWidget *description_label = gtk_label_new(task->description);
        gtk_box_pack_start(GTK_BOX(row), description_label, TRUE, TRUE, 0);

        GtkWidget *progress_bar = gtk_progress_bar_new();
        if (strcmp(task->status, "not started") == 0) {
            gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), 0.0);
            gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress_bar), "Not Started");
        } else if (strcmp(task->status, "inprogress") == 0) {
            gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), 0.5);
            gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress_bar), "In Progress");
        } else if (strcmp(task->status, "completed") == 0) {
            gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), 1.0);
            gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress_bar), "Completed");
        }
        gtk_box_pack_start(GTK_BOX(row), progress_bar, TRUE, TRUE, 0);

        GtkWidget *button = gtk_button_new_with_label("View Details");
        char *user_data = g_strdup_printf("%s;%s;%s;%d;%d", task->task_name, task->description, task->status, task->assignee_id, task->task_id);
        g_signal_connect(button, "clicked", G_CALLBACK(on_task_clicked), user_data);
        gtk_box_pack_start(GTK_BOX(row), button, FALSE, FALSE, 0);

        gtk_list_box_insert(GTK_LIST_BOX(list_box), row, -1);
    }

    return scroll_window;
}


// Callback khi tạo task mới
void on_create_task_clicked(GtkButton *button, gpointer user_data) {
    ProjectContext *context = (ProjectContext *)user_data;
    GtkWidget *project_window = context->project_window;
    int project_id = context->project_id;

    GtkWidget *dialog = gtk_dialog_new_with_buttons("Tạo Task",
                                                    GTK_WINDOW(project_window),
                                                    GTK_DIALOG_MODAL,
                                                    "Tạo", GTK_RESPONSE_ACCEPT,
                                                    "Hủy", GTK_RESPONSE_CANCEL,
                                                    NULL);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_add(GTK_CONTAINER(content_area), grid);

    GtkWidget *task_name_label = gtk_label_new("Tên Task:");
    GtkWidget *task_name_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), task_name_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), task_name_entry, 1, 0, 1, 1);

    GtkWidget *description_label = gtk_label_new("Mô tả:");
    GtkWidget *description_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), description_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), description_entry, 1, 1, 1, 1);

    GtkWidget *assignee_label = gtk_label_new("Assignee ID:");
    GtkWidget *assignee_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), assignee_label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), assignee_entry, 1, 2, 1, 1);

    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        const char *task_name = gtk_entry_get_text(GTK_ENTRY(task_name_entry));
        const char *description = gtk_entry_get_text(GTK_ENTRY(description_entry));
        int assignee_id = atoi(gtk_entry_get_text(GTK_ENTRY(assignee_entry)));

        if (send_create_task(task_name, description, project_id, assignee_id)) {
            g_print("Task '%s' đã được tạo thành công.\n", task_name);
            refresh_project_window(project_window, context);
        } else {
            g_print("Không thể tạo task '%s'.\n", task_name);
        }
    }
    gtk_widget_destroy(dialog);
}

void on_add_member_clicked(GtkButton *button, gpointer user_data) {
    ProjectContext *context = (ProjectContext *)user_data;
    int project_id = context->project_id;
    GtkWidget *members_list_box = GTK_WIDGET(context->project_window);

    GtkWidget *dialog = gtk_dialog_new_with_buttons("Thêm Thành Viên",
                                                    GTK_WINDOW(context->project_window),
                                                    GTK_DIALOG_MODAL,
                                                    "Thêm", GTK_RESPONSE_ACCEPT,
                                                    "Hủy", GTK_RESPONSE_CANCEL,
                                                    NULL);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_add(GTK_CONTAINER(content_area), grid);

    // Trường nhập User ID
    GtkWidget *user_id_label = gtk_label_new("User ID:");
    GtkWidget *user_id_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), user_id_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), user_id_entry, 1, 0, 1, 1);

    // Trường nhập Role
    GtkWidget *role_label = gtk_label_new("Role:");
    GtkWidget *role_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), role_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), role_entry, 1, 1, 1, 1);

    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        const char *user_id_text = gtk_entry_get_text(GTK_ENTRY(user_id_entry));
        const char *role_text = gtk_entry_get_text(GTK_ENTRY(role_entry));
        int user_id = atoi(user_id_text);

        if (send_add_member_request(project_id, user_id, role_text)) {
            g_print("Member '%d' với role '%s' đã được thêm thành công.\n", user_id, role_text);
            // Làm mới danh sách thành viên
            load_members(members_list_box, project_id);
        } else {
            g_print("Không thể thêm member '%d' với role '%s'.\n", user_id, role_text);
        }
    }

    gtk_widget_destroy(dialog);
}

void load_members(GtkWidget *list_box, int project_id) {
    char response[4096];

    if (!send_get_members_request(project_id, response, sizeof(response))) {
        GtkWidget *error_label = gtk_label_new("Failed to load members.");
        gtk_list_box_insert(GTK_LIST_BOX(list_box), error_label, -1);
        return;
    }

    const char *start = response;
    const char *end;

    while ((end = strchr(start, '\n')) != NULL) {
        size_t length = end - start;
        char member_info[256];
        strncpy(member_info, start, length);
        member_info[length] = '\0';

        GtkWidget *row = gtk_label_new(member_info);
        gtk_list_box_insert(GTK_LIST_BOX(list_box), row, -1);

        start = end + 1;
    }

    gtk_widget_show_all(list_box);
}


// Làm mới giao diện
void refresh_project_window(GtkWidget *project_window, ProjectContext *context) {
    GtkWidget *vbox = gtk_bin_get_child(GTK_BIN(project_window));
    GList *children = gtk_container_get_children(GTK_CONTAINER(vbox));

    for (GList *child = children; child != NULL; child = child->next) {
        gtk_widget_destroy(GTK_WIDGET(child->data));
    }
    g_list_free(children);

    TaskList task_list_data = {0};
    if (!load_tasks_from_server(context->project_id, &task_list_data)) {
        GtkWidget *error_label = gtk_label_new("Failed to load tasks.");
        gtk_box_pack_start(GTK_BOX(vbox), error_label, TRUE, TRUE, 0);
    } else {
        GtkWidget *combined_view = create_combined_task_view(&task_list_data);
        gtk_box_pack_start(GTK_BOX(vbox), combined_view, TRUE, TRUE, 0);
    }

    GtkWidget *create_task_button = gtk_button_new_with_label("Tạo Task");
    gtk_box_pack_start(GTK_BOX(vbox), create_task_button, FALSE, FALSE, 0);
    g_signal_connect(create_task_button, "clicked", G_CALLBACK(on_create_task_clicked), context);

    GtkWidget *chat_button = gtk_button_new_with_label("Chat");
    gtk_box_pack_start(GTK_BOX(vbox), chat_button, FALSE, FALSE, 0);
    g_signal_connect(chat_button, "clicked", G_CALLBACK(on_chat_button_clicked), context);

    GtkWidget *members_list_box = gtk_list_box_new();
    gtk_box_pack_start(GTK_BOX(vbox), members_list_box, TRUE, TRUE, 0);
    load_members(members_list_box, context->project_id);

    GtkWidget *add_member_button = gtk_button_new_with_label("Thêm Thành Viên");
    gtk_box_pack_start(GTK_BOX(vbox), add_member_button, FALSE, FALSE, 0);
    g_signal_connect(add_member_button, "clicked", G_CALLBACK(on_add_member_clicked), context);

    gtk_widget_show_all(project_window);
}

// Hàm tạo cửa sổ project
GtkWidget *create_project_window(int project_id) {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Project Tasks and Gantt Chart");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    ProjectContext *context = g_malloc(sizeof(ProjectContext));
    context->project_id = project_id;
    context->project_window = window;

    TaskList task_list_data = {0};
    if (!load_tasks_from_server(project_id, &task_list_data)) {
        GtkWidget *error_label = gtk_label_new("Failed to load tasks.");
        gtk_box_pack_start(GTK_BOX(vbox), error_label, TRUE, TRUE, 0);
    } else {
        GtkWidget *combined_view = create_combined_task_view(&task_list_data);
        gtk_box_pack_start(GTK_BOX(vbox), combined_view, TRUE, TRUE, 0);
    }

    GtkWidget *create_task_button = gtk_button_new_with_label("Tạo Task");
    gtk_box_pack_start(GTK_BOX(vbox), create_task_button, FALSE, FALSE, 0);
    g_signal_connect(create_task_button, "clicked", G_CALLBACK(on_create_task_clicked), context);

    GtkWidget *chat_button = gtk_button_new_with_label("Chat");
    gtk_box_pack_start(GTK_BOX(vbox), chat_button, FALSE, FALSE, 0);
    g_signal_connect(chat_button, "clicked", G_CALLBACK(on_chat_button_clicked), context);

// Tạo danh sách thành viên
    GtkWidget *members_list_box = gtk_list_box_new();
    gtk_box_pack_start(GTK_BOX(vbox), members_list_box, TRUE, TRUE, 0);
    load_members(members_list_box, project_id);

    // Nút thêm thành viên
    GtkWidget *add_member_button = gtk_button_new_with_label("Thêm Thành Viên");
    gtk_box_pack_start(GTK_BOX(vbox), add_member_button, FALSE, FALSE, 0);
    g_signal_connect(add_member_button, "clicked", G_CALLBACK(on_add_member_clicked), context);
    // Đảm bảo giải phóng context khi cửa sổ bị hủy
    // g_signal_connect(window, "destroy", G_CALLBACK(on_project_window_destroy), context);


    gtk_widget_show_all(window);
    return window;
}
