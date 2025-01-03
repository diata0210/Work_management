#include "task_window.h"
#include "../network/socket.h"
#include "../message_handlers/data_message_handler.h"
#include <stdio.h>

bool send_download_request(int file_id, const char *save_path) {
    char request[256];
    snprintf(request, sizeof(request), "DATA DOWNLOAD %d", file_id);

    char response[4096];
    if (send_request(request, response)) {
        // Giả định phản hồi từ server là nội dung file
        FILE *file = fopen(save_path, "wb");
        if (file) {
            fwrite(response, 1, strlen(response), file);
            fclose(file);
            return true;
        }
    }
    return false;
}

void on_download_clicked(GtkButton *button, gpointer user_data) {
    int file_id = GPOINTER_TO_INT(user_data);

    char file_path[256];
    snprintf(file_path, sizeof(file_path), "downloads/file_%d", file_id);

    if (send_download_request(file_id, file_path)) {
        g_print("File %d đã được tải xuống và lưu tại: %s\n", file_id, file_path);
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
                                                   GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_MESSAGE_INFO,
                                                   GTK_BUTTONS_OK,
                                                   "File đã được tải xuống: %s", file_path);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    } else {
        g_print("Không thể tải xuống file %d.\n", file_id);
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
                                                   GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_MESSAGE_ERROR,
                                                   GTK_BUTTONS_CLOSE,
                                                   "Lỗi khi tải file.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}


GtkWidget *create_comment_view(int task_id) {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *label = gtk_label_new("Comments:");
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);

    char response[4096];
    if (send_get_comments_request(task_id, response, sizeof(response))) {
        const char *start = response + 9; // Bỏ "COMMENTS\n"
        const char *end;
        while ((end = strchr(start, '\n')) != NULL) {
            size_t length = end - start;
            char line[256];
            strncpy(line, start, length);
            line[length] = '\0';

            GtkWidget *comment_label = gtk_label_new(line);
            gtk_box_pack_start(GTK_BOX(vbox), comment_label, FALSE, FALSE, 0);
            start = end + 1;
        }
    } else {
        GtkWidget *no_comment_label = gtk_label_new("No comments available.");
        gtk_box_pack_start(GTK_BOX(vbox), no_comment_label, FALSE, FALSE, 0);
    }

    GtkWidget *add_comment_button = gtk_button_new_with_label("Thêm Comment");
    g_signal_connect(add_comment_button, "clicked", G_CALLBACK(on_add_comment_clicked), GINT_TO_POINTER(task_id));
    gtk_box_pack_start(GTK_BOX(vbox), add_comment_button, FALSE, FALSE, 0);

    return vbox;
}



// GtkWidget *create_attachment_view(int task_id) {
//     GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
//     GtkWidget *label = gtk_label_new("Attachments:");
//     gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);

//     char response[4096];
//     if (send_get_attachments_request(task_id, response, sizeof(response))) {
//         const char *start = response + 12; // Bỏ "ATTACHMENTS\n"
//         const char *end;
//         while ((end = strchr(start, '\n')) != NULL) {
//             size_t length = end - start;
//             char line[256];
//             strncpy(line, start, length);
//             line[length] = '\0';

//             GtkWidget *attachment_label = gtk_label_new(line);
//             gtk_box_pack_start(GTK_BOX(vbox), attachment_label, FALSE, FALSE, 0);
//             start = end + 1;
//         }
//     } else {
//         GtkWidget *no_attachment_label = gtk_label_new("No attachments available.");
//         gtk_box_pack_start(GTK_BOX(vbox), no_attachment_label, FALSE, FALSE, 0);
//     }

//     GtkWidget *add_attachment_button = gtk_button_new_with_label("Thêm File");
//     g_signal_connect(add_attachment_button, "clicked", G_CALLBACK(on_add_attachment_clicked), GINT_TO_POINTER(task_id));
//     gtk_box_pack_start(GTK_BOX(vbox), add_attachment_button, FALSE, FALSE, 0);

//     return vbox;
// }
GtkWidget *create_attachment_view(int task_id) {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *label = gtk_label_new("Attachments:");
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);

    char response[4096];
    if (send_get_attachments_request(task_id, response, sizeof(response))) {
        const char *start = response + 12; // Bỏ "ATTACHMENTS\n"
        const char *end;
        while ((end = strchr(start, '\n')) != NULL) {
            size_t length = end - start;
            char line[256];
            strncpy(line, start, length);
            line[length] = '\0';

            // Tách thông tin file (giả định định dạng "FILE_ID: %d, FILE_NAME: %s")
            int file_id;
            char file_name[128];
            if (sscanf(line, "FILE_ID: %d, FILE_NAME: %127s", &file_id, file_name) == 2) {
                GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

                // Tên file
                GtkWidget *file_label = gtk_label_new(file_name);
                gtk_box_pack_start(GTK_BOX(hbox), file_label, FALSE, FALSE, 0);

                // Nút download
                GtkWidget *download_button = gtk_button_new_with_label("Download");
                g_signal_connect(download_button, "clicked", G_CALLBACK(on_download_clicked), GINT_TO_POINTER(file_id));
                gtk_box_pack_start(GTK_BOX(hbox), download_button, FALSE, FALSE, 0);

                gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
            }

            start = end + 1;
        }
    } else {
        GtkWidget *no_attachment_label = gtk_label_new("No attachments available.");
        gtk_box_pack_start(GTK_BOX(vbox), no_attachment_label, FALSE, FALSE, 0);
    }

    GtkWidget *add_attachment_button = gtk_button_new_with_label("Thêm File");
    g_signal_connect(add_attachment_button, "clicked", G_CALLBACK(on_add_attachment_clicked), GINT_TO_POINTER(task_id));
    gtk_box_pack_start(GTK_BOX(vbox), add_attachment_button, FALSE, FALSE, 0);

    return vbox;
}


void on_update_status_clicked(GtkButton *button, gpointer user_data) {
    int task_id = GPOINTER_TO_INT(user_data);

    GtkWidget *dialog = gtk_dialog_new_with_buttons("Cập nhật trạng thái",
                                                    NULL,
                                                    GTK_DIALOG_MODAL,
                                                    "Cập nhật", GTK_RESPONSE_ACCEPT,
                                                    "Hủy", GTK_RESPONSE_CANCEL,
                                                    NULL);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    // Tạo combo box để chọn trạng thái
    GtkWidget *status_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(status_combo), NULL, "not started");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(status_combo), NULL, "inprogress");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(status_combo), NULL, "completed");

    gtk_box_pack_start(GTK_BOX(content_area), status_combo, FALSE, FALSE, 0);
    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        const char *selected_status = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(status_combo));

        if (send_update_status_request(task_id, selected_status)) {
            g_print("Trạng thái của Task %d đã được cập nhật thành '%s'.\n", task_id, selected_status);
        } else {
            g_print("Không thể cập nhật trạng thái của Task %d.\n", task_id);
        }
    }

    gtk_widget_destroy(dialog);
}

void reset_comment_view(GtkWidget *comment_view, int task_id) {
    // Xóa toàn bộ các widget con hiện tại
    GList *children = gtk_container_get_children(GTK_CONTAINER(comment_view));
    for (GList *iter = children; iter != NULL; iter = iter->next) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    // Thêm lại label tiêu đề
    GtkWidget *label = gtk_label_new("Comments:");
    gtk_box_pack_start(GTK_BOX(comment_view), label, FALSE, FALSE, 0);

    // Lấy dữ liệu từ server
    char response[4096];
    if (send_get_comments_request(task_id, response, sizeof(response))) {
        const char *start = response;
        const char *end;
        while ((end = strchr(start, '\n')) != NULL) {
            size_t length = end - start;
            char line[256];
            strncpy(line, start, length);
            line[length] = '\0';

            GtkWidget *comment_label = gtk_label_new(line);
            gtk_box_pack_start(GTK_BOX(comment_view), comment_label, FALSE, FALSE, 0);
            start = end + 1;
        }
    }

    // Thêm lại nút thêm comment
    GtkWidget *add_comment_button = gtk_button_new_with_label("Thêm Comment");
    g_signal_connect(add_comment_button, "clicked", G_CALLBACK(on_add_comment_clicked), GINT_TO_POINTER(task_id));
    gtk_box_pack_start(GTK_BOX(comment_view), add_comment_button, FALSE, FALSE, 0);

    // Hiển thị lại toàn bộ widget
    gtk_widget_show_all(comment_view);
}
// void reset_attachment_view(GtkWidget *attachment_view, int task_id) {
//     // Xóa toàn bộ các widget con hiện tại
//     GList *children = gtk_container_get_children(GTK_CONTAINER(attachment_view));
//     for (GList *iter = children; iter != NULL; iter = iter->next) {
//         gtk_widget_destroy(GTK_WIDGET(iter->data));
//     }
//     g_list_free(children);

//     // Thêm lại label tiêu đề
//     GtkWidget *label = gtk_label_new("Attachments:");
//     gtk_box_pack_start(GTK_BOX(attachment_view), label, FALSE, FALSE, 0);

//     // Lấy dữ liệu từ server
//     char response[4096];
//     if (send_get_attachments_request(task_id, response, sizeof(response))) {
//         const char *start = response;
//         const char *end;
//         while ((end = strchr(start, '\n')) != NULL) {
//             size_t length = end - start;
//             char line[256];
//             strncpy(line, start, length);
//             line[length] = '\0';

//             GtkWidget *attachment_label = gtk_label_new(line);
//             gtk_box_pack_start(GTK_BOX(attachment_view), attachment_label, FALSE, FALSE, 0);
//             start = end + 1;
//         }
//     }

//     // Thêm lại nút thêm file đính kèm
//     GtkWidget *add_attachment_button = gtk_button_new_with_label("Thêm File");
//     g_signal_connect(add_attachment_button, "clicked", G_CALLBACK(on_add_attachment_clicked), GINT_TO_POINTER(task_id));
//     gtk_box_pack_start(GTK_BOX(attachment_view), add_attachment_button, FALSE, FALSE, 0);

//     // Hiển thị lại toàn bộ widget
//     gtk_widget_show_all(attachment_view);
// }
void reset_attachment_view(GtkWidget *attachment_view, int task_id) {
    // Xóa toàn bộ các widget con hiện tại
    GList *children = gtk_container_get_children(GTK_CONTAINER(attachment_view));
    for (GList *iter = children; iter != NULL; iter = iter->next) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    // Thêm lại label tiêu đề
    GtkWidget *label = gtk_label_new("Attachments:");
    gtk_box_pack_start(GTK_BOX(attachment_view), label, FALSE, FALSE, 0);

    // Lấy dữ liệu từ server
    char response[4096];
    if (send_get_attachments_request(task_id, response, sizeof(response))) {
        const char *start = response + 12; // Bỏ "ATTACHMENTS\n"
        const char *end;
        while ((end = strchr(start, '\n')) != NULL) {
            size_t length = end - start;
            char line[256];
            strncpy(line, start, length);
            line[length] = '\0';

            // Tách thông tin file (giả định định dạng "FILE_ID: %d, FILE_NAME: %s")
            int file_id;
            char file_name[128];
            if (sscanf(line, "FILE_ID: %d, FILE_NAME: %127s", &file_id, file_name) == 2) {
                GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

                // Tên file
                GtkWidget *file_label = gtk_label_new(file_name);
                gtk_box_pack_start(GTK_BOX(hbox), file_label, FALSE, FALSE, 0);

                // Nút download
                GtkWidget *download_button = gtk_button_new_with_label("Download");
                g_signal_connect(download_button, "clicked", G_CALLBACK(on_download_clicked), GINT_TO_POINTER(file_id));
                gtk_box_pack_start(GTK_BOX(hbox), download_button, FALSE, FALSE, 0);

                gtk_box_pack_start(GTK_BOX(attachment_view), hbox, FALSE, FALSE, 0);
            }

            start = end + 1;
        }
    } else {
        GtkWidget *no_attachment_label = gtk_label_new("No attachments available.");
        gtk_box_pack_start(GTK_BOX(attachment_view), no_attachment_label, FALSE, FALSE, 0);
    }

    // Thêm lại nút thêm file đính kèm
    GtkWidget *add_attachment_button = gtk_button_new_with_label("Thêm File");
    g_signal_connect(add_attachment_button, "clicked", G_CALLBACK(on_add_attachment_clicked), GINT_TO_POINTER(task_id));
    gtk_box_pack_start(GTK_BOX(attachment_view), add_attachment_button, FALSE, FALSE, 0);

    // Hiển thị lại toàn bộ widget
    gtk_widget_show_all(attachment_view);
}

void on_add_comment_clicked(GtkButton *button, gpointer user_data) {
    int task_id = GPOINTER_TO_INT(user_data);

    GtkWidget *dialog = gtk_dialog_new_with_buttons("Thêm Comment",
                                                    NULL,
                                                    GTK_DIALOG_MODAL,
                                                    "Thêm", GTK_RESPONSE_ACCEPT,
                                                    "Hủy", GTK_RESPONSE_CANCEL,
                                                    NULL);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *comment_entry = gtk_entry_new();
    gtk_container_add(GTK_CONTAINER(content_area), comment_entry);
    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        const char *comment_content = gtk_entry_get_text(GTK_ENTRY(comment_entry));
        if (send_add_comment(task_id, comment_content)) {
            g_print("Comment đã được thêm thành công.\n");

            // Reset lại comment view
            GtkWidget *comment_view = gtk_widget_get_parent(GTK_WIDGET(button));
            reset_comment_view(comment_view, task_id);
        } else {
            g_print("Không thể thêm comment.\n");
        }
    }

    gtk_widget_destroy(dialog);
}
// void on_add_attachment_clicked(GtkButton *button, gpointer user_data) {
//     int task_id = GPOINTER_TO_INT(user_data);

//     GtkWidget *dialog = gtk_file_chooser_dialog_new("Chọn File",
//                                                     NULL,
//                                                     GTK_FILE_CHOOSER_ACTION_OPEN,
//                                                     "Hủy", GTK_RESPONSE_CANCEL,
//                                                     "Thêm", GTK_RESPONSE_ACCEPT,
//                                                     NULL);

//     if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
//         GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
//         char *file_path = gtk_file_chooser_get_filename(chooser);
//         char *file_name = g_path_get_basename(file_path);

//         if (send_add_attachment(task_id, file_name, file_path)) {
//             g_print("File đính kèm đã được thêm thành công.\n");

//             // Reset lại attachment view
//             GtkWidget *attachment_view = gtk_widget_get_parent(GTK_WIDGET(button));
//             reset_attachment_view(attachment_view, task_id);
//         } else {
//             g_print("Không thể thêm file đính kèm.\n");
//         }

//         g_free(file_path);
//         g_free(file_name);
//     }

//     gtk_widget_destroy(dialog);
// }


void on_add_attachment_clicked(GtkButton *button, gpointer user_data) {
    int task_id = GPOINTER_TO_INT(user_data);

    // Tạo dialog chọn file
    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Chọn File",
        NULL,
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "Hủy", GTK_RESPONSE_CANCEL,
        "Thêm", GTK_RESPONSE_ACCEPT,
        NULL
    );

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        char *file_path = gtk_file_chooser_get_filename(chooser);
        char *file_name = g_path_get_basename(file_path);

        // Gửi file đến server
        if (send_add_attachment(task_id, file_name, file_path)) {
            g_print("File đính kèm đã được thêm thành công.\n");

            // Reset lại attachment view
            GtkWidget *attachment_view = gtk_widget_get_parent(GTK_WIDGET(button));
            reset_attachment_view(attachment_view, task_id);
        } else {
            g_print("Không thể thêm file đính kèm.\n");
        }

        // Giải phóng tài nguyên
        g_free(file_path);
        g_free(file_name);
    }

    gtk_widget_destroy(dialog);
}


// Callback cho nút cập nhật tiến độ
void on_update_progress_clicked(GtkButton *button, gpointer user_data) {
    int task_id = GPOINTER_TO_INT(user_data);
    g_print("Update Progress button clicked for Task ID: %d\n", task_id);

    // Hiển thị hộp thoại nhập tiến độ
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        "Update Progress",
        NULL,
        GTK_DIALOG_MODAL,
        "Update", GTK_RESPONSE_ACCEPT,
        "Cancel", GTK_RESPONSE_CANCEL,
        NULL
    );

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Enter progress percentage (e.g., 50)");
    gtk_container_add(GTK_CONTAINER(content_area), entry);
    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        const char *progress = gtk_entry_get_text(GTK_ENTRY(entry));
        g_print("Task ID: %d - New Progress: %s%%\n", task_id, progress);
        // Gửi tiến độ mới đến server
    }

    gtk_widget_destroy(dialog);
}

// Callback cho nút thêm comment
// void on_add_comment_clicked(GtkButton *button, gpointer user_data) {
//     int task_id = GPOINTER_TO_INT(user_data);
//     g_print("Add Comment button clicked for Task ID: %d\n", task_id);

//     // Hiển thị hộp thoại nhập comment
//     GtkWidget *dialog = gtk_dialog_new_with_buttons(
//         "Add Comment",
//         NULL,
//         GTK_DIALOG_MODAL,
//         "Add", GTK_RESPONSE_ACCEPT,
//         "Cancel", GTK_RESPONSE_CANCEL,
//         NULL
//     );

//     GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
//     GtkWidget *entry = gtk_entry_new();
//     gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Enter your comment");
//     gtk_container_add(GTK_CONTAINER(content_area), entry);
//     gtk_widget_show_all(dialog);

//     if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
//         const char *comment = gtk_entry_get_text(GTK_ENTRY(entry));
//         g_print("Task ID: %d - New Comment: %s\n", task_id, comment);
//         // Gửi comment mới đến server
//     }

//     gtk_widget_destroy(dialog);
// }

// Callback cho nút thêm file đính kèm
// void on_add_attachment_clicked(GtkButton *button, gpointer user_data) {
//     int task_id = GPOINTER_TO_INT(user_data);
//     g_print("Add Attachment button clicked for Task ID: %d\n", task_id);

//     // Hiển thị hộp thoại chọn file
//     GtkWidget *dialog = gtk_file_chooser_dialog_new(
//         "Add Attachment",
//         NULL,
//         GTK_FILE_CHOOSER_ACTION_OPEN,
//         "_Cancel", GTK_RESPONSE_CANCEL,
//         "_Open", GTK_RESPONSE_ACCEPT,
//         NULL
//     );

//     if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
//         char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
//         g_print("Task ID: %d - Selected file: %s\n", task_id, filename);
//         // Gửi file đính kèm đến server
//         g_free(filename);
//     }

//     gtk_widget_destroy(dialog);
// }

// Hàm tạo giao diện chi tiết task
// GtkWidget *create_task_window(const char *task_name, const char *description, const char *status, int assignee_id, int task_id) {
//     // Tạo cửa sổ
//     GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//     gtk_window_set_title(GTK_WINDOW(window), "Task Details");
//     gtk_window_set_default_size(GTK_WINDOW(window), 400, 400);

//     // Tạo container chính
//     GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
//     gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
//     gtk_container_add(GTK_CONTAINER(window), vbox);

//     // Hiển thị tên task
//     GtkWidget *name_label = gtk_label_new(NULL);
//     gtk_label_set_text(GTK_LABEL(name_label), task_name);
//     gtk_box_pack_start(GTK_BOX(vbox), name_label, FALSE, FALSE, 0);

//     // Hiển thị mô tả task
//     GtkWidget *description_label = gtk_label_new(NULL);
//     gtk_label_set_text(GTK_LABEL(description_label), description);
//     gtk_box_pack_start(GTK_BOX(vbox), description_label, FALSE, FALSE, 0);

//     // Hiển thị trạng thái task
//     GtkWidget *status_label = gtk_label_new(NULL);
//     char status_text[256];
//     snprintf(status_text, sizeof(status_text), "Status: %s", status);
//     gtk_label_set_text(GTK_LABEL(status_label), status_text);
//     gtk_box_pack_start(GTK_BOX(vbox), status_label, FALSE, FALSE, 0);

//     // Hiển thị thông tin người được giao task
//     GtkWidget *assignee_label = gtk_label_new(NULL);
//     char assignee_text[256];
//     snprintf(assignee_text, sizeof(assignee_text), "Assigned to: %d", assignee_id);
//     gtk_label_set_text(GTK_LABEL(assignee_label), assignee_text);
//     gtk_box_pack_start(GTK_BOX(vbox), assignee_label, FALSE, FALSE, 0);

//     GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
// gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);

// GtkWidget *comment_view = create_comment_view(task_id);
//     gtk_box_pack_start(GTK_BOX(hbox), comment_view, TRUE, TRUE, 0);

//     // Khung file đính kèm
//     GtkWidget *attachment_view = create_attachment_view(task_id);
//     gtk_box_pack_start(GTK_BOX(hbox), attachment_view, TRUE, TRUE, 0);

//     // Thêm nút "Cập nhật tiến độ"
//     GtkWidget *update_progress_button = gtk_button_new_with_label("Update Progress");
//     g_signal_connect(update_progress_button, "clicked", G_CALLBACK(on_update_progress_clicked), GINT_TO_POINTER(task_id));
//     gtk_box_pack_start(GTK_BOX(vbox), update_progress_button, FALSE, FALSE, 0);

//     // // Thêm nút "Thêm comment"
//     // GtkWidget *add_comment_button = gtk_button_new_with_label("Add Comment");
//     // g_signal_connect(add_comment_button, "clicked", G_CALLBACK(on_add_comment_clicked), GINT_TO_POINTER(task_id));
//     // gtk_box_pack_start(GTK_BOX(vbox), add_comment_button, FALSE, FALSE, 0);

//     // // Thêm nút "Thêm file đính kèm"
//     // GtkWidget *add_attachment_button = gtk_button_new_with_label("Add Attachment");
//     // g_signal_connect(add_attachment_button, "clicked", G_CALLBACK(on_add_attachment_clicked), GINT_TO_POINTER(task_id));
//     // gtk_box_pack_start(GTK_BOX(vbox), add_attachment_button, FALSE, FALSE, 0);

//     return window;
// }
GtkWidget *create_task_window(const char *task_name, const char *description, const char *status, int assignee_id, int task_id) {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Chi tiết Task");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    GtkWidget *content_area = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), content_area);

    // Hiển thị thông tin task
    char task_info[512];
    snprintf(task_info, sizeof(task_info), "Task: %s\nDescription: %s\nStatus: %s\nAssigned To: %d", 
             task_name, description, status, assignee_id);
    GtkWidget *task_info_label = gtk_label_new(task_info);
    gtk_box_pack_start(GTK_BOX(content_area), task_info_label, FALSE, FALSE, 0);

    // Nút cập nhật trạng thái
    GtkWidget *update_status_button = gtk_button_new_with_label("Cập nhật trạng thái");
    g_signal_connect(update_status_button, "clicked", G_CALLBACK(on_update_status_clicked), GINT_TO_POINTER(task_id));
    gtk_box_pack_start(GTK_BOX(content_area), update_status_button, FALSE, FALSE, 0);

    // Khung comment và file đính kèm
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(content_area), hbox, TRUE, TRUE, 0);

    GtkWidget *comment_view = create_comment_view(task_id);
    gtk_box_pack_start(GTK_BOX(hbox), comment_view, TRUE, TRUE, 0);

    GtkWidget *attachment_view = create_attachment_view(task_id);
    gtk_box_pack_start(GTK_BOX(hbox), attachment_view, TRUE, TRUE, 0);

    gtk_widget_show_all(window);
    return window;
}
