#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <gtk/gtk.h>
#include <gst/gst.h>

// Forward declarations at the top
void create_home_window();
void handle_logout(GtkWidget *button, gpointer window);
void show_message_dialog(GtkWindow *parent, const char *message);
gboolean check_login(GtkWidget *widget);

// Thêm struct để lưu trữ thông tin người dùng
typedef struct {
    int id;
    char username[50];
    gboolean is_logged_in;
} UserInfo;

// Biến toàn cục
int client_socket;
UserInfo current_user = {0, "", FALSE};

// Thêm struct để truyền nhiều tham số qua callback
typedef struct {
    GtkWidget **entries;
    int entry_count;
    GtkWidget *dialog;
    GtkWidget *status_label;
} CallbackData;

void show_message_dialog(GtkWindow *parent, const char *message) {
    GtkWidget *dialog = gtk_message_dialog_new(parent,
                                             GTK_DIALOG_DESTROY_WITH_PARENT,
                                             GTK_MESSAGE_INFO,
                                             GTK_BUTTONS_OK,
                                             "%s", message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void send_request(const char *request, char *response) {
    send(client_socket, request, strlen(request), 0);
    int valread = read(client_socket, response, 1024);
    response[valread] = '\0';
}

void send_request_and_show_message(const char *request, GtkWindow *parent) {
    char response[1024];
    send_request(request, response);
    show_message_dialog(parent, response);
}

// Sửa lại các hàm callback
void on_register_clicked(GtkWidget *widget, gpointer data) {
    CallbackData *cb_data = (CallbackData *)data;
    const char *username = gtk_entry_get_text(GTK_ENTRY(cb_data->entries[0]));
    const char *password = gtk_entry_get_text(GTK_ENTRY(cb_data->entries[1]));
    const char *email = gtk_entry_get_text(GTK_ENTRY(cb_data->entries[2]));

    if (!validate_input(username, password, GTK_WINDOW(cb_data->dialog))) {
        return;
    }

    // Validate email
    if (strchr(email, '@') == NULL) {
        show_message_dialog(GTK_WINDOW(cb_data->dialog), "Invalid email address");
        return;
    }

    char request[256], response[1024];
    sprintf(request, "REGISTER %s %s %s", username, password, email);
    send_request(request, response);
    
    if(strcmp(response, "Register successful") == 0) {
        gtk_widget_destroy(GTK_WIDGET(cb_data->dialog));
        show_message_dialog(NULL, "Registration successful! Please login.");
    }
}

// Add global reference to main window grid and function buttons
GtkWidget *main_grid = NULL;
GtkWidget *function_buttons[8] = {NULL}; // Array to hold function buttons

void show_function_buttons(gboolean show) {
    for(int i = 0; i < 8; i++) {
        if(function_buttons[i]) {
            if(show) {
                gtk_widget_show(function_buttons[i]);
            } else {
                gtk_widget_hide(function_buttons[i]);
            }
        }
    }
}

void on_login_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget **entries = (GtkWidget **)data;
    const char *username = gtk_entry_get_text(GTK_ENTRY(entries[0]));
    const char *password = gtk_entry_get_text(GTK_ENTRY(entries[1]));

    // Log để debug
    printf("Attempting login - Username: %s\n", username);

    if (!validate_input(username, password, GTK_WINDOW(gtk_widget_get_toplevel(widget)))) {
        return;
    }

    char request[256];
    char response[1024];
    sprintf(request, "LOGIN %s %s", username, password);
    printf("Sending request: %s\n", request); // Log request
    
    send_request(request, response);
    printf("Received response: %s\n", response); // Log response
    
    if(strcmp(response, "Login successful") == 0) {
        strcpy(current_user.username, username);
        current_user.is_logged_in = TRUE;
        gtk_widget_destroy(gtk_widget_get_toplevel(widget));
        create_home_window();
    } else {
        show_message_dialog(GTK_WINDOW(gtk_widget_get_toplevel(widget)), 
            "Login failed - Invalid username or password");
    }
}

void on_create_project_clicked(GtkWidget *widget, gpointer data) {
    const char *project_name = gtk_entry_get_text(GTK_ENTRY(((GtkWidget **)data)[0]));
    const char *description = gtk_entry_get_text(GTK_ENTRY(((GtkWidget **)data)[1]));

    char request[256];
    sprintf(request, "CREATE_PROJECT 1 %s %s", project_name, description);
    send_request_and_show_message(request, GTK_WINDOW(gtk_widget_get_toplevel(widget)));
}

void on_create_task_clicked(GtkWidget *widget, gpointer data) {
    const char *project_id = gtk_entry_get_text(GTK_ENTRY(((GtkWidget **)data)[0]));
    const char *assigned_to = gtk_entry_get_text(GTK_ENTRY(((GtkWidget **)data)[1]));
    const char *task_name = gtk_entry_get_text(GTK_ENTRY(((GtkWidget **)data)[2]));
    const char *status = gtk_entry_get_text(GTK_ENTRY(((GtkWidget **)data)[3]));

    char request[256];
    sprintf(request, "CREATE_TASK %s %s %s %s", project_id, assigned_to, task_name, status);
    send_request_and_show_message(request, GTK_WINDOW(gtk_widget_get_toplevel(widget)));
}

void on_update_task_status_clicked(GtkWidget *widget, gpointer data) {
    const char *task_id = gtk_entry_get_text(GTK_ENTRY(((GtkWidget **)data)[0]));
    const char *status = gtk_entry_get_text(GTK_ENTRY(((GtkWidget **)data)[1]));

    char request[256];
    sprintf(request, "UPDATE_TASK_STATUS %s %s", task_id, status);
    send_request_and_show_message(request, GTK_WINDOW(gtk_widget_get_toplevel(widget)));
}

void on_add_comment_clicked(GtkWidget *widget, gpointer data) {
    const char *task_id = gtk_entry_get_text(GTK_ENTRY(((GtkWidget **)data)[0]));
    const char *comment = gtk_entry_get_text(GTK_ENTRY(((GtkWidget **)data)[1]));

    char request[256];
    sprintf(request, "ADD_COMMENT %s 1 %s", task_id, comment);
    send_request_and_show_message(request, GTK_WINDOW(gtk_widget_get_toplevel(widget)));
}

void on_chat_clicked(GtkWidget *widget, gpointer data) {
    const char *message = gtk_entry_get_text(GTK_ENTRY(data));

    char request[256];
    sprintf(request, "CHAT 1 1 %s", message);
    send_request_and_show_message(request, GTK_WINDOW(gtk_widget_get_toplevel(widget)));
}

void on_video_call_clicked(GtkWidget *widget, gpointer data) {
    if (!check_login(main_grid)) return;
    char response[1024];
    send_request("VIDEO_CALL", response);

    // Initialize GStreamer
    gst_init(NULL, NULL);

    // Create the elements
    GstElement *pipeline = gst_pipeline_new("video-call-pipeline");
    GstElement *source = gst_element_factory_make("videotestsrc", "source");
    GstElement *sink = gst_element_factory_make("autovideosink", "sink");

    if (!pipeline || !source || !sink) {
        g_printerr("Not all elements could be created.\n");
        return;
    }

    // Build the pipeline
    gst_bin_add_many(GST_BIN(pipeline), source, sink, NULL);
    if (gst_element_link(source, sink) != TRUE) {
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(pipeline);
        return;
    }

    // Start playing
    GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Unable to set the pipeline to the playing state.\n");
        gst_object_unref(pipeline);
        return;
    }

    // Wait until error or EOS
    GstBus *bus = gst_element_get_bus(pipeline);
    GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    // Parse message
    if (msg != NULL) {
        GError *err;
        gchar *debug_info;

        switch (GST_MESSAGE_TYPE(msg)) {
            case GST_MESSAGE_ERROR:
                gst_message_parse_error(msg, &err, &debug_info);
                g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
                g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
                g_clear_error(&err);
                g_free(debug_info);
                break;
            case GST_MESSAGE_EOS:
                g_print("End-Of-Stream reached.\n");
                break;
            default:
                // We should not reach here because we only asked for ERRORs and EOS
                g_printerr("Unexpected message received.\n");
                break;
        }
        gst_message_unref(msg);
    }

    // Free resources
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
}

void on_add_attachment_clicked(GtkWidget *widget, gpointer data) {
    const char *task_id = gtk_entry_get_text(GTK_ENTRY(((GtkWidget **)data)[0]));
    const char *file_path = gtk_entry_get_text(GTK_ENTRY(((GtkWidget **)data)[1]));

    char request[256];
    sprintf(request, "ADD_ATTACHMENT %s %s", task_id, file_path);
    send_request_and_show_message(request, GTK_WINDOW(gtk_widget_get_toplevel(widget)));
}

void on_view_gantt_chart_clicked(GtkWidget *widget, gpointer data) {
    char response[1024];
    send_request("VIEW_GANTT_CHART", response);
    show_message_dialog(GTK_WINDOW(gtk_widget_get_toplevel(widget)), response);
}

GtkWidget *create_entry_with_label(GtkWidget *grid, const char *label_text, int row) {
    GtkWidget *label = gtk_label_new(label_text);
    GtkWidget *entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry, 1, row, 1, 1);
    return entry;
}

void create_register_window() {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Register");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    GtkWidget *username_entry = create_entry_with_label(grid, "Username:", 0);
    GtkWidget *password_entry = create_entry_with_label(grid, "Password:", 1);
    GtkWidget *email_entry = create_entry_with_label(grid, "Email:", 2);

    // Cấp phát bộ nhớ cho CallbackData
    CallbackData *cb_data = g_new(CallbackData, 1);
    cb_data->entries = g_new(GtkWidget*, 3);
    cb_data->entries[0] = username_entry;
    cb_data->entries[1] = password_entry;
    cb_data->entries[2] = email_entry;
    cb_data->entry_count = 3;
    cb_data->dialog = window;

    GtkWidget *register_button = gtk_button_new_with_label("Register");
    g_signal_connect(register_button, "clicked", G_CALLBACK(on_register_clicked), cb_data);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_widget_destroy), NULL);
    g_signal_connect_swapped(window, "destroy", G_CALLBACK(g_free), cb_data->entries);
    g_signal_connect_swapped(window, "destroy", G_CALLBACK(g_free), cb_data);

    gtk_grid_attach(GTK_GRID(grid), register_button, 0, 3, 2, 1);
    gtk_widget_show_all(window);
}

void create_login_window() {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Login");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_widget_destroy), NULL);

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    GtkWidget *username_entry = create_entry_with_label(grid, "Username:", 0);
    GtkWidget *password_entry = create_entry_with_label(grid, "Password:", 1);
    
    // Cài đặt password entry để ẩn mật khẩu
    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(password_entry), '*');

    // Allocate memory for entries array that persists
    GtkWidget **entries = g_new(GtkWidget*, 2);
    entries[0] = username_entry;
    entries[1] = password_entry;

    GtkWidget *login_button = gtk_button_new_with_label("Login");
    g_signal_connect(login_button, "clicked", G_CALLBACK(on_login_clicked), entries);
    g_signal_connect_swapped(window, "destroy", G_CALLBACK(g_free), entries);
    gtk_grid_attach(GTK_GRID(grid), login_button, 0, 2, 2, 1);

    gtk_widget_show_all(window);
}

// Add helper function to check login status
gboolean check_login(GtkWidget *widget) {
    if (!current_user.is_logged_in) {
        show_message_dialog(GTK_WINDOW(gtk_widget_get_toplevel(widget)), 
            "Please login first");
        return FALSE;
    }
    return TRUE;
}

// Update create_project_window with proper memory management
void create_project_window() {
    if (!check_login(main_grid)) return;
    
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Create Project");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_widget_destroy), NULL);

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    GtkWidget *project_name_entry = create_entry_with_label(grid, "Project Name:", 0);
    GtkWidget *description_entry = create_entry_with_label(grid, "Description:", 1);

    // Allocate memory for entries that persists
    GtkWidget **entries = g_new(GtkWidget*, 2);
    entries[0] = project_name_entry;
    entries[1] = description_entry;

    GtkWidget *create_project_button = gtk_button_new_with_label("Create Project");
    g_signal_connect(create_project_button, "clicked", G_CALLBACK(on_create_project_clicked), entries);
    g_signal_connect_swapped(window, "destroy", G_CALLBACK(g_free), entries);
    
    gtk_grid_attach(GTK_GRID(grid), create_project_button, 0, 2, 2, 1);

    gtk_widget_show_all(window);
}

// Similarly update other window creation functions
void create_task_window() {
    if (!check_login(main_grid)) return;
    
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Create Task");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_widget_destroy), NULL);

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    GtkWidget *project_id_entry = create_entry_with_label(grid, "Project ID:", 0);
    GtkWidget *assigned_to_entry = create_entry_with_label(grid, "Assigned To:", 1);
    GtkWidget *task_name_entry = create_entry_with_label(grid, "Task Name:", 2);
    GtkWidget *status_entry = create_entry_with_label(grid, "Status:", 3);

    GtkWidget **entries = g_new(GtkWidget*, 4);
    entries[0] = project_id_entry;
    entries[1] = assigned_to_entry;
    entries[2] = task_name_entry;
    entries[3] = status_entry;

    GtkWidget *create_task_button = gtk_button_new_with_label("Create Task");
    g_signal_connect(create_task_button, "clicked", G_CALLBACK(on_create_task_clicked), entries);
    g_signal_connect_swapped(window, "destroy", G_CALLBACK(g_free), entries);
    gtk_grid_attach(GTK_GRID(grid), create_task_button, 0, 4, 2, 1);

    gtk_widget_show_all(window);
}

void update_task_status_window() {
    if (!check_login(main_grid)) return;
    
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Update Task Status");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_widget_destroy), NULL);

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    GtkWidget *task_id_entry = create_entry_with_label(grid, "Task ID:", 0);
    GtkWidget *status_entry = create_entry_with_label(grid, "Status:", 1);

    GtkWidget **entries = g_new(GtkWidget*, 2);
    entries[0] = task_id_entry;
    entries[1] = status_entry;

    GtkWidget *update_task_status_button = gtk_button_new_with_label("Update Task Status");
    g_signal_connect(update_task_status_button, "clicked", G_CALLBACK(on_update_task_status_clicked), entries);
    g_signal_connect_swapped(window, "destroy", G_CALLBACK(g_free), entries);
    gtk_grid_attach(GTK_GRID(grid), update_task_status_button, 0, 2, 2, 1);

    gtk_widget_show_all(window);
}

void add_comment_window() {
    if (!check_login(main_grid)) return;
    
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Add Comment");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_widget_destroy), NULL);

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    GtkWidget *task_id_entry = create_entry_with_label(grid, "Task ID:", 0);
    GtkWidget *comment_entry = create_entry_with_label(grid, "Comment:", 1);

    GtkWidget **entries = g_new(GtkWidget*, 2);
    entries[0] = task_id_entry;
    entries[1] = comment_entry;

    GtkWidget *add_comment_button = gtk_button_new_with_label("Add Comment");
    g_signal_connect(add_comment_button, "clicked", G_CALLBACK(on_add_comment_clicked), entries);
    g_signal_connect_swapped(window, "destroy", G_CALLBACK(g_free), entries);
    gtk_grid_attach(GTK_GRID(grid), add_comment_button, 0, 2, 2, 1);

    gtk_widget_show_all(window);
}

void add_attachment_window() {
    if (!check_login(main_grid)) return;
    
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Add Attachment");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_widget_destroy), NULL);

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    GtkWidget *task_id_entry = create_entry_with_label(grid, "Task ID:", 0);
    GtkWidget *file_path_entry = create_entry_with_label(grid, "File Path:", 1);

    GtkWidget **entries = g_new(GtkWidget*, 2);
    entries[0] = task_id_entry;
    entries[1] = file_path_entry;

    GtkWidget *add_attachment_button = gtk_button_new_with_label("Add Attachment");
    g_signal_connect(add_attachment_button, "clicked", G_CALLBACK(on_add_attachment_clicked), entries);
    g_signal_connect_swapped(window, "destroy", G_CALLBACK(g_free), entries);
    gtk_grid_attach(GTK_GRID(grid), add_attachment_button, 0, 2, 2, 1);

    gtk_widget_show_all(window);
}

void view_gantt_chart_window() {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Gantt Chart");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_widget_destroy), NULL);

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    GtkWidget *view_gantt_chart_button = gtk_button_new_with_label("View Gantt Chart");
    g_signal_connect(view_gantt_chart_button, "clicked", G_CALLBACK(on_view_gantt_chart_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), view_gantt_chart_button, 0, 0, 2, 1);

    gtk_widget_show_all(window);
}

void chat_window() {
    if (!check_login(main_grid)) return;
    
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Chat");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_widget_destroy), NULL);

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    GtkWidget *message_entry = create_entry_with_label(grid, "Message:", 0);

    GtkWidget **entries = g_new(GtkWidget*, 1);
    entries[0] = message_entry;

    GtkWidget *chat_button = gtk_button_new_with_label("Chat");
    g_signal_connect(chat_button, "clicked", G_CALLBACK(on_chat_clicked), message_entry);
    g_signal_connect_swapped(window, "destroy", G_CALLBACK(g_free), entries);
    gtk_grid_attach(GTK_GRID(grid), chat_button, 0, 1, 2, 1);

    gtk_widget_show_all(window);
}

// Add CSS styling function
void apply_custom_styles() {
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "window { background-color: #f8f9fc; }"
        ".title-label { font-size: 24px; font-weight: bold; color: #2e59d9; margin: 10px; }"
        ".login-button, .register-button { "
        "  background: linear-gradient(to bottom, #4e73df, #224abe);"
        "  color: white;"
        "  border-radius: 5px;"
        "  border: none;"
        "  padding: 10px;"
        "  font-weight: bold;"
        "  transition: all 0.3s ease;"
        "}"
        ".login-button:hover, .register-button:hover {"
        "  background: linear-gradient(to bottom, #224abe, #1a3a9c);"
        "  transform: translateY(-2px);"
        "}"
        ".function-button {"
        "  background: linear-gradient(to bottom, #4e73df, #224abe);"
        "  color: white;"
        "  border-radius: 8px;"
        "  border: none;"
        "  padding: 15px;"
        "  margin: 5px;"
        "  font-weight: bold;"
        "  transition: all 0.3s ease;"
        "}"
        ".function-button:hover {"
        "  background: linear-gradient(to bottom, #224abe, #1a3a9c);"
        "  transform: translateY(-2px);"
        "  box-shadow: 0 5px 15px rgba(0,0,0,0.2);"
        "}"
        ".header-label { font-size: 18px; font-weight: bold; color: #5a5c69; }"
        ".entry { border-radius: 4px; padding: 5px; }"
        , -1, NULL);
    
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

// Update the create_main_window function
void create_main_window() {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Project Management System - Login");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_container_set_border_width(GTK_CONTAINER(main_box), 40);
    gtk_container_add(GTK_CONTAINER(window), main_box);

    // Add welcome message
    GtkWidget *welcome_label = gtk_label_new("Welcome to Project Management System");
    gtk_box_pack_start(GTK_BOX(main_box), welcome_label, FALSE, FALSE, 10);

    // Add login and register buttons
    GtkWidget *btn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
    
    GtkWidget *login_button = gtk_button_new_with_label("Login");
    GtkWidget *register_button = gtk_button_new_with_label("Register");
    
    gtk_widget_set_size_request(login_button, 120, 40);
    gtk_widget_set_size_request(register_button, 120, 40);
    
    g_signal_connect(login_button, "clicked", G_CALLBACK(create_login_window), NULL);
    g_signal_connect(register_button, "clicked", G_CALLBACK(create_register_window), NULL);
    
    gtk_box_pack_start(GTK_BOX(btn_box), login_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(btn_box), register_button, TRUE, TRUE, 0);
    
    gtk_box_pack_start(GTK_BOX(main_box), btn_box, TRUE, TRUE, 0);

    // Apply custom styles
    apply_custom_styles();

    GtkStyleContext *context = gtk_widget_get_style_context(welcome_label);
    gtk_style_context_add_class(context, "title-label");
    
    // Style login/register buttons
    context = gtk_widget_get_style_context(login_button);
    gtk_style_context_add_class(context, "login-button");
    
    context = gtk_widget_get_style_context(register_button);
    gtk_style_context_add_class(context, "register-button");

    gtk_widget_show_all(window);
}

// Add new functions for home page
void create_home_window() {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Project Management System");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create main container
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), main_box);
    
    // Add header with welcome message and username
    GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    char welcome_msg[100];
    sprintf(welcome_msg, "Welcome, %s!", current_user.username);
    GtkWidget *welcome_label = gtk_label_new(welcome_msg);
    gtk_box_pack_start(GTK_BOX(header), welcome_label, TRUE, TRUE, 5);
    
    // Add logout button
    GtkWidget *logout_btn = gtk_button_new_with_label("Logout");
    g_signal_connect(logout_btn, "clicked", G_CALLBACK(handle_logout), window);
    gtk_box_pack_end(GTK_BOX(header), logout_btn, FALSE, FALSE, 5);
    
    gtk_box_pack_start(GTK_BOX(main_box), header, FALSE, FALSE, 5);
    
    // Add main content area with grid layout
    GtkWidget *content_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(content_grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(content_grid), 10);
    gtk_container_set_border_width(GTK_CONTAINER(content_grid), 20);
    
    // Create styled buttons for each function
    const char *button_labels[] = {
        "Create Project", "Create Task", "Update Task Status", "Add Comment",
        "Add Attachment", "View Gantt Chart", "Chat", "Video Call"
    };
    
    void (*button_handlers[])(GtkWidget*, gpointer) = {
        create_project_window, create_task_window, update_task_status_window,
        add_comment_window, add_attachment_window, view_gantt_chart_window,
        chat_window, on_video_call_clicked
    };

    for (int i = 0; i < 8; i++) {
        GtkWidget *button = gtk_button_new_with_label(button_labels[i]);
        gtk_widget_set_size_request(button, 150, 100);
        
        // Style the button
        GtkStyleContext *context = gtk_widget_get_style_context(button);
        gtk_style_context_add_class(context, "function-button");
        
        g_signal_connect(button, "clicked", G_CALLBACK(button_handlers[i]), NULL);
        gtk_grid_attach(GTK_GRID(content_grid), button, i % 4, i / 4, 1, 1);
    }
    
    gtk_box_pack_start(GTK_BOX(main_box), content_grid, TRUE, TRUE, 0);
    
    // Add CSS styling
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        ".function-button {"
        "  background: linear-gradient(to bottom, #4e73df, #224abe);"
        "  color: white;"
        "  border-radius: 5px;"
        "  border: none;"
        "  padding: 10px;"
        "}"
        ".function-button:hover {"
        "  background: linear-gradient(to bottom, #224abe, #1a3a9c);"
        "}", -1, NULL);
    
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    
    gtk_widget_show_all(window);
}

void handle_logout(GtkWidget *button, gpointer window) {
    current_user.is_logged_in = FALSE;
    strcpy(current_user.username, "");
    gtk_widget_destroy(GTK_WIDGET(window));
    create_main_window();
}

// Add input validation for login/register
gboolean validate_input(const char *username, const char *password, GtkWindow *parent) {
    if (strlen(username) < 3) {
        show_message_dialog(parent, "Username must be at least 3 characters long");
        return FALSE;
    }
    if (strlen(password) < 6) {
        show_message_dialog(parent, "Password must be at least 6 characters long");
        return FALSE;
    }
    return TRUE;
}

int main(int argc, char *argv[]) {
    struct sockaddr_in server_addr;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    gtk_init(&argc, &argv);
    apply_custom_styles();  // Apply styles globally
    create_main_window();
    gtk_main();

    close(client_socket);
    return 0;
}