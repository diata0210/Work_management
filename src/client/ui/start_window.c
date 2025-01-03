#include <gtk/gtk.h>
#include "login_window.h"
#include "register_window.h"

// Callback khi nhấn nút "Login"
static void on_login_button_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *login_window = create_login_window();
    gtk_widget_show_all(login_window);
    gtk_widget_destroy(GTK_WIDGET(user_data)); // Đóng cửa sổ start_window
}

// Callback khi nhấn nút "Register"
static void on_register_button_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *register_window = create_register_window();
    gtk_widget_show_all(register_window);
    gtk_widget_destroy(GTK_WIDGET(user_data)); // Đóng cửa sổ start_window
}

// Tạo giao diện start_window
GtkWidget *create_start_window() {
    GtkWidget *start_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(start_window), "Welcome");
    gtk_window_set_default_size(GTK_WINDOW(start_window), 300, 200);
    gtk_container_set_border_width(GTK_CONTAINER(start_window), 10);

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(start_window), grid);

    GtkWidget *welcome_label = gtk_label_new("Welcome! Please choose an option:");
    GtkWidget *login_button = gtk_button_new_with_label("Login");
    GtkWidget *register_button = gtk_button_new_with_label("Register");

    gtk_grid_attach(GTK_GRID(grid), welcome_label, 0, 0, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), login_button, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), register_button, 1, 1, 1, 1);

    // Gắn callback cho các nút
    g_signal_connect(login_button, "clicked", G_CALLBACK(on_login_button_clicked), start_window);
    g_signal_connect(register_button, "clicked", G_CALLBACK(on_register_button_clicked), start_window);

    return start_window;
}
