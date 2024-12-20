// src/client/ui/login_window.c

#include <gtk/gtk.h>
#include "../include/client.h"
#include "../include/auth.h"
#include "main_window.h"

typedef struct {
    GtkWidget *window;
    GtkWidget *username_entry;
    GtkWidget *password_entry;
    GtkWidget *login_button;
    GtkWidget *register_button;
    GtkWidget *status_label;
    Client *client;
} LoginWindow;

static void on_login_clicked(GtkButton *button, gpointer user_data) {
    LoginWindow *login = (LoginWindow*)user_data;
    
    const char *username = gtk_entry_get_text(GTK_ENTRY(login->username_entry));
    const char *password = gtk_entry_get_text(GTK_ENTRY(login->password_entry));
    
    if (strlen(username) == 0 || strlen(password) == 0) {
        gtk_label_set_text(GTK_LABEL(login->status_label),
                          "Please enter username and password");
        return;
    }
    
    int result = auth_login(login->client, username, password);
    if (result == 0) {
        // Login successful
        gtk_widget_destroy(login->window);
        create_main_window(login->client);
    } else {
        // Login failed
        gtk_label_set_text(GTK_LABEL(login->status_label),
                          "Login failed. Please check your credentials.");
    }
}

static void on_register_clicked(GtkButton *button, gpointer user_data) {
    LoginWindow *login = (LoginWindow*)user_data;
    
    const char *username = gtk_entry_get_text(GTK_ENTRY(login->username_entry));
    const char *password = gtk_entry_get_text(GTK_ENTRY(login->password_entry));
    
    if (strlen(username) == 0 || strlen(password) == 0) {
        gtk_label_set_text(GTK_LABEL(login->status_label),
                          "Please enter username and password");
        return;
    }
    
    int result = auth_register(login->client, username, password);
    if (result == 0) {
        gtk_label_set_text(GTK_LABEL(login->status_label),
                          "Registration successful. Please login.");
    } else {
        gtk_label_set_text(GTK_LABEL(login->status_label),
                          "Registration failed. Username may be taken.");
    }
}

LoginWindow* create_login_window(Client *client) {
    LoginWindow *login = g_new0(LoginWindow, 1);
    login->client = client;

    // Create main window
    login->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(login->window), "Login");
    gtk_window_set_position(GTK_WINDOW(login->window), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(login->window), 10);

    // Create layout
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);

    // Create widgets
    GtkWidget *username_label = gtk_label_new("Username:");
    login->username_entry = gtk_entry_new();
    
    GtkWidget *password_label = gtk_label_new("Password:");
    login->password_entry = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(login->password_entry), FALSE);
    gtk_entry_set_input_purpose(GTK_ENTRY(login->password_entry), 
                               GTK_INPUT_PURPOSE_PASSWORD);

    login->login_button = gtk_button_new_with_label("Login");
    login->register_button = gtk_button_new_with_label("Register");
    login->status_label = gtk_label_new("");
    gtk_label_set_line_wrap(GTK_LABEL(login->status_label), TRUE);

    // Add widgets to grid
    gtk_grid_attach(GTK_GRID(grid), username_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), login->username_entry, 1, 0, 2, 1);
    
    gtk_grid_attach(GTK_GRID(grid), password_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), login->password_entry, 1, 1, 2, 1);
    
    gtk_grid_attach(GTK_GRID(grid), login->login_button, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), login->register_button, 2, 2, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), login->status_label, 0, 3, 3, 1);

    // Connect signals
    g_signal_connect(G_OBJECT(login->login_button), "clicked",
                    G_CALLBACK(on_login_clicked), login);
    g_signal_connect(G_OBJECT(login->register_button), "clicked",
                    G_CALLBACK(on_register_clicked), login);
    g_signal_connect(G_OBJECT(login->window), "destroy",
                    G_CALLBACK(gtk_main_quit), NULL);

    // Add grid to window
    gtk_container_add(GTK_CONTAINER(login->window), grid);

    gtk_widget_show_all(login->window);
    return login;
}
