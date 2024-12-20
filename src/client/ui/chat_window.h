#ifndef CHAT_WINDOW_H
#define CHAT_WINDOW_H

#include <gtk/gtk.h>
#include "../include/client.h"
#include "../message_handlers/chat_message_handler.h"

// Định nghĩa cấu trúc cho cửa sổ chat
typedef struct {
    GtkWidget *window;               // Cửa sổ chính của cửa sổ chat
    GtkWidget *messages_view;        // View hiển thị tin nhắn
    GtkTextBuffer *messages_buffer;  // Buffer chứa tin nhắn
    GtkWidget *entry;                // Entry để nhập tin nhắn
    GtkWidget *send_button;          // Nút gửi tin nhắn
    int chat_id;                     // ID của cuộc trò chuyện
} ChatWindow;

// Khai báo các hàm liên quan đến chat window
GtkWidget* create_chat_window(int chat_id);
void display_message(ChatWindow *chat_window, const char *message);
void send_message(ChatWindow *chat_window);
void close_chat_window(ChatWindow *chat_window);

#endif // CHAT_WINDOW_H
