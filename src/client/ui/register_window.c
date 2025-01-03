#include <gtk/gtk.h>
#include "../network/socket.h"
#include "../message_handlers/control_message_handler.h"
#include "register_window.h"
#include "start_window.h"

#define BUFFER_SIZE 1024

// Callback khi nhấn nút Back
static void on_back_button_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *register_window = GTK_WIDGET(user_data);
    gtk_widget_destroy(register_window);

    // Hiển thị màddfdfn hình chính
    GtkWidget *start_window = create_start_window();
    if (start_window != NULL) {
        gtk_widget_show_all(start_window);
    } else {
        g_print("Failed to create start window.\n");
    }
}

// Callback khi nhấn nút Register
static void on_register_button_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget **entries = (GtkWidget **)user_data;
    if (entries == NULL) {
        g_print("Entries array is NULL. Exiting callback.\n");
        return;
    }

    GtkWidget *username_entry = entries[0];
    GtkWidget *password_entry = entries[1];

    if (username_entry == NULL || password_entry == NULL) {
        g_print("Username or password entry is NULL. Exiting callback.\n");
        return;
    }

    const char *username = gtk_entry_get_text(GTK_ENTRY(username_entry));
    const char *password = gtk_entry_get_text(GTK_ENTRY(password_entry));

    if (strlen(username) == 0 || strlen(password) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_CLOSE,
            "Username and password cannot be empty!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    char request[512];
    snprintf(request, sizeof(request), "CONTROL REGISTER %s %s", username, password);

    char response[BUFFER_SIZE];
    if (send_request(request, response)) {
        if (strcmp(response, "REGISTER_SUCCESS") == 0) {
            GtkWidget *dialog = gtk_message_dialog_new(NULL,
                GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_INFO,
                GTK_BUTTONS_CLOSE,
                "Registration successful! Please log in.");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);

            GtkWidget *register_window = gtk_widget_get_toplevel(GTK_WIDGET(button));
            gtk_widget_destroy(register_window);

            GtkWidget *start_window = create_start_window();
            if (start_window != NULL) {
                gtk_widget_show_all(start_window);
            } else {
                g_print("Failed to create start window.\n");
            }
        } else {
            GtkWidget *dialog = gtk_message_dialog_new(NULL,
                GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_ERROR,
                GTK_BUTTONS_CLOSE,
                "Registration failed. Please try again.");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
        }
    } else {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_CLOSE,
            "Failed to communicate with server. Please try again.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}

// Hàm tạo giao diện đăng ký
GtkWidget *create_register_window() {
    GtkWidget *register_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(register_window), "Register");
    gtk_window_set_default_size(GTK_WINDOW(register_window), 300, 200);
    gtk_container_set_border_width(GTK_CONTAINER(register_window), 10);

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(register_window), grid);

    GtkWidget *username_label = gtk_label_new("Username:");
    GtkWidget *username_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(username_entry), "Enter your username");

    GtkWidget *password_label = gtk_label_new("Password:");
    GtkWidget *password_entry = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);
    gtk_entry_set_placeholder_text(GTK_ENTRY(password_entry), "Enter your password");

    GtkWidget *register_button = gtk_button_new_with_label("Register");
    GtkWidget *back_button = gtk_button_new_with_label("Back");

    gtk_grid_attach(GTK_GRID(grid), username_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), username_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), password_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), password_entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), register_button, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), back_button, 1, 2, 1, 1);

    GtkWidget **entries = g_malloc(2 * sizeof(GtkWidget *));
    entries[0] = username_entry;
    entries[1] = password_entry;

    g_signal_connect(register_button, "clicked", G_CALLBACK(on_register_button_clicked), entries);
    g_signal_connect(back_button, "clicked", G_CALLBACK(on_back_button_clicked), register_window);

    return register_window;
}
