#include <gtk/gtk.h>
#include "../include/auth.h"
#include "../message_handlers/control_message_handler.h"
#include <string.h>
#include "../network/socket.h"
#include "login_window.h"
#include "main_window.h"

#define BUFFER_SIZE 1024

// Các biến toàn cục cho username và password entry
GtkWidget *username_entry;
GtkWidget *password_entry;
GtkWidget *login_window;

// Callback khi nhấn nút Login
static void on_login_button_clicked(GtkButton *button, gpointer user_data) {
    const char *username = gtk_entry_get_text(GTK_ENTRY(username_entry));
    const char *password = gtk_entry_get_text(GTK_ENTRY(password_entry));

    // Kiểm tra nếu username hoặc password trống
    if (strlen(username) == 0 || strlen(password) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(login_window),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_CLOSE,
            "Username and password cannot be empty!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    // Tạo yêu cầu login
    char request[512];
    snprintf(request, sizeof(request), "CONTROL LOGIN %s %s", username, password);

    // Gửi yêu cầu qua socket và nhận phản hồi
    char response[BUFFER_SIZE];
    if (send_request(request, response)) {
        if (strcmp(response, "LOGIN_SUCCESS") == 0) {
            // Đóng cửa sổ đăng nhập
            gtk_widget_destroy(login_window);

            // Hiển thị cửa sổ chính
            GtkWidget *main_window = create_main_window();
            gtk_widget_show_all(main_window);
        } else {
            GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(login_window),
                GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_ERROR,
                GTK_BUTTONS_CLOSE,
                "Login failed. Please check your credentials.");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
        }
    } else {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(login_window),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_CLOSE,
            "Failed to communicate with server. Please try again.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}

// Hàm tạo giao diện đăng nhập
GtkWidget *create_login_window() {
    login_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(login_window), "Login");
    gtk_window_set_default_size(GTK_WINDOW(login_window), 300, 200);
    gtk_container_set_border_width(GTK_CONTAINER(login_window), 10);

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(login_window), grid);

    GtkWidget *username_label = gtk_label_new("Username:");
    username_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(username_entry), "Enter your username");

    GtkWidget *password_label = gtk_label_new("Password:");
    password_entry = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);
    gtk_entry_set_placeholder_text(GTK_ENTRY(password_entry), "Enter your password");

    GtkWidget *login_button = gtk_button_new_with_label("Login");

    gtk_grid_attach(GTK_GRID(grid), username_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), username_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), password_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), password_entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), login_button, 0, 2, 2, 1);

    // Gắn callback cho nút Login
    g_signal_connect(login_button, "clicked", G_CALLBACK(on_login_button_clicked), NULL);

    return login_window;
}
