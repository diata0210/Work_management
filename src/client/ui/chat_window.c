
// #include <gtk/gtk.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include "../network/socket.h"
// #include "../message_handlers/chat_message_handler.h"
// gboolean update_chat_history(gpointer user_data) {
//     if (!user_data) return FALSE; // Kiểm tra NULL

//     const char *data = (const char *)user_data;
//     GtkTextView *chat_history = GTK_TEXT_VIEW(g_object_get_data(G_OBJECT(gtk_widget_get_toplevel(GTK_WIDGET(user_data))), "chat_history"));
//     if (!chat_history) {
//         g_free(user_data); // Giải phóng bộ nhớ
//         return FALSE;
//     }

//     GtkTextBuffer *buffer = gtk_text_view_get_buffer(chat_history);
//     GtkTextIter end_iter;
//     gtk_text_buffer_get_end_iter(buffer, &end_iter);
//     gtk_text_buffer_insert(buffer, &end_iter, data, -1);
//     gtk_text_buffer_insert(buffer, &end_iter, "\n", -1);

//     printf("Message received: %s\n", data);
//     g_free(user_data); // Giải phóng bộ nhớ
//     return FALSE;      // Chỉ chạy một lần
// }


// void handle_received_data(const char *data) {
//     if (data == NULL || strlen(data) == 0) return;

//     // Tạo bản sao dữ liệu vì g_idle_add yêu cầu dữ liệu tồn tại khi nó chạy
//     char *message = g_strdup(data);

//     // Thêm công việc vào hàng đợi của luồng chính GTK
//     g_idle_add((GSourceFunc)update_chat_history, message);
// }



// void on_send_message_clicked(GtkButton *button, gpointer user_data) {
//     GtkWidget *entry = GTK_WIDGET(user_data);
//     const char *message = gtk_entry_get_text(GTK_ENTRY(entry));

//     if (message == NULL || strlen(message) == 0) {
//         printf("Error: Message is empty.\n");
//         return;
//     }

//     // Lấy project_id từ cửa sổ hiện tại
//     GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(entry));
//     int project_id = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(window), "project_id"));

//     // Gửi tin nhắn đến server
//     char formatted_message[1024]; // Tạo một buffer để chứa tin nhắn định dạng
//     char response[4096];
//     memset(response, 0, sizeof(response));
// // Định dạng tin nhắn theo cấu trúc "CHAT project_id content"
//     snprintf(formatted_message, sizeof(formatted_message), "CHAT %d %s", project_id, message);
//     printf("Message sent: %s\n", formatted_message);    
// // Gửi tin nhắn đã được định dạng
//     if (send_request(formatted_message,response)) {
//         printf("Response: %s\n", response);
//         if(strcmp(response,"CHAT_RECEIVED") == 0){
//             printf("Message sent successfully.\n");
//         }else{
//             printf("Error: Failed to send message.\n");
//         }
//         // Cập nhật khung nhập tin nhắn
//         gtk_entry_set_text(GTK_ENTRY(entry), "");
//     } else {
//         printf("Error: Failed to send message.\n");
//     }

// }
// // void load_chat_history(int project_id, GtkWidget *chat_history) {
// //     char request[256];
// //     char response[4096];

// //     snprintf(request, sizeof(request), "CHAT GET_HISTORY %d", project_id);

// //     if (send_request(request, response)) {
// //         GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(chat_history));
// //         gtk_text_buffer_set_text(buffer, response, -1);
// //     } else {
// //         GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(chat_history));
// //         gtk_text_buffer_set_text(buffer, "Failed to load chat history.", -1);
// //     }
// // }
// // Hàm tạo màn hình chat
// GtkWidget *create_chat_window(int project_id) {
//     // Tạo cửa sổ chat
//     GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//     gtk_window_set_title(GTK_WINDOW(window), "Chat");
//     gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);

//     // Lưu trữ project_id vào cửa sổ
//     g_object_set_data(G_OBJECT(window), "project_id", GINT_TO_POINTER(project_id));

//     // Tạo container chính
//     GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
//     gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
//     gtk_container_add(GTK_CONTAINER(window), vbox);

//     // Tạo khung hiển thị lịch sử chat
//     GtkWidget *chat_history = gtk_text_view_new();
//     gtk_text_view_set_editable(GTK_TEXT_VIEW(chat_history), FALSE);
//     gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(chat_history), FALSE);
//     GtkWidget *chat_scrolled = gtk_scrolled_window_new(NULL, NULL);
//     gtk_container_add(GTK_CONTAINER(chat_scrolled), chat_history);
//     gtk_box_pack_start(GTK_BOX(vbox), chat_scrolled, TRUE, TRUE, 0);

//     // Lưu trữ tham chiếu lịch sử chat
//     g_object_set_data(G_OBJECT(window), "chat_history", chat_history);

//     // Tạo khung nhập tin nhắn
//     GtkWidget *entry = gtk_entry_new();
//     gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 0);

//     // Tạo nút gửi tin nhắn
//     GtkWidget *send_button = gtk_button_new_with_label("Send");
//     gtk_box_pack_start(GTK_BOX(vbox), send_button, FALSE, FALSE, 0);

//     // Load lịch sử chat
//     // load_chat_history(project_id, chat_history);

//     // Gắn callback cho nút gửi tin nhắn
//     g_signal_connect(send_button, "clicked", G_CALLBACK(on_send_message_clicked), (gpointer)entry);

//     // Khởi động luồng nhận dữ liệu từ server
//     // if (!receive_data_async(handle_received_data)) {
//     //     fprintf(stderr, "Failed to start receive thread.\n");
//     // }
//     if (!receive_data_async(handle_received_data)) {
//     fprintf(stderr, "Failed to start receive thread.\n");
// }
//     gtk_widget_show_all(window);
//     return window;
// }
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFF_SIZE 1024

int client_socket;
char buff[BUFF_SIZE];

// Hàm xử lý nhận dữ liệu từ server
void *receive_handler(void *arg) {
    GtkTextView *chat_history = GTK_TEXT_VIEW(arg);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(chat_history);
    GtkTextIter end_iter;

    while (1) {
        char buff[BUFF_SIZE];
        if (!receive_response(buff, BUFF_SIZE)) {
            fprintf(stderr, "Error: Failed to receive data or connection closed. Exiting receive loop.\n");
            pthread_exit(NULL);
        }
        printf("Received: %s\n", buff);

        gtk_text_buffer_get_end_iter(buffer, &end_iter);
        gtk_text_buffer_insert(buffer, &end_iter, buff, -1);
        gtk_text_buffer_insert(buffer, &end_iter, "\n", -1);
    }

    return NULL;
}

// Hàm gửi tin nhắn
void send_message(const char *message, int project_id) {
    char formatted_message[BUFF_SIZE];
    snprintf(formatted_message, sizeof(formatted_message), "CHAT %d %s", project_id, message);
    printf("Sending: %s\n", formatted_message);

    if (!send_request_no_response(formatted_message)) {
        fprintf(stderr, "Error: Failed to send message.\n");
        exit(EXIT_FAILURE);
    }
}

// Callback khi bấm nút gửi tin nhắn
void on_send_message_clicked(GtkButton *button, gpointer user_data) {
    GtkEntry *entry = GTK_ENTRY(user_data);
    const char *message = gtk_entry_get_text(entry);

    if (message == NULL || strlen(message) == 0) {
        printf("Error: Message is empty.\n");
        return;
    }

    // Lấy project_id từ dữ liệu gắn vào cửa sổ chính
    GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(entry));
    int project_id = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(window), "project_id"));

    send_message(message, project_id);
    gtk_entry_set_text(entry, "");
}

// Hàm tạo màn hình chat
GtkWidget *create_chat_window(int project_id) {
    // Tạo cửa sổ chat
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Chat");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);

    // Lưu trữ project_id vào cửa sổ
    g_object_set_data(G_OBJECT(window), "project_id", GINT_TO_POINTER(project_id));

    // Tạo container chính
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Tạo khung hiển thị lịch sử chat
    GtkWidget *chat_history = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(chat_history), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(chat_history), FALSE);
    GtkWidget *chat_scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(chat_scrolled), chat_history);
    gtk_box_pack_start(GTK_BOX(vbox), chat_scrolled, TRUE, TRUE, 0);

    // Tạo khung nhập tin nhắn
    GtkWidget *entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 0);

    // Tạo nút gửi tin nhắn
    GtkWidget *send_button = gtk_button_new_with_label("Send");
    gtk_box_pack_start(GTK_BOX(vbox), send_button, FALSE, FALSE, 0);

    // Gắn callback cho nút gửi tin nhắn
    g_signal_connect(send_button, "clicked", G_CALLBACK(on_send_message_clicked), entry);

    // Tạo luồng nhận dữ liệu
    pthread_t receive_thread;
    pthread_create(&receive_thread, NULL, receive_handler, chat_history);

    gtk_widget_show_all(window);
    return window;
}
