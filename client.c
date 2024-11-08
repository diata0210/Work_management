#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <gtk/gtk.h>
#include <gst/gst.h>

int client_socket;

void send_request(const char *request) {
    char buffer[1024];
    send(client_socket, request, strlen(request), 0);
    int valread = read(client_socket, buffer, 1024);
    buffer[valread] = '\0';
    printf("%s\n", buffer);
}

void on_register_clicked(GtkWidget *widget, gpointer data) {
    send_request("REGISTER username password");
}

void on_login_clicked(GtkWidget *widget, gpointer data) {
    send_request("LOGIN username password");
}

void on_create_project_clicked(GtkWidget *widget, gpointer data) {
    send_request("CREATE_PROJECT project_name");
}

void on_create_task_clicked(GtkWidget *widget, gpointer data) {
    send_request("CREATE_TASK task_name");
}

void on_update_task_status_clicked(GtkWidget *widget, gpointer data) {
    send_request("UPDATE_TASK_STATUS task_id status");
}

void on_add_comment_clicked(GtkWidget *widget, gpointer data) {
    send_request("ADD_COMMENT task_id comment");
}

void on_chat_clicked(GtkWidget *widget, gpointer data) {
    send_request("CHAT message");
}

void on_video_call_clicked(GtkWidget *widget, gpointer data) {
    send_request("VIDEO_CALL");

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

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Project Management Client");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    GtkWidget *register_button = gtk_button_new_with_label("Register");
    g_signal_connect(register_button, "clicked", G_CALLBACK(on_register_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), register_button, 0, 0, 1, 1);

    GtkWidget *login_button = gtk_button_new_with_label("Login");
    g_signal_connect(login_button, "clicked", G_CALLBACK(on_login_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), login_button, 1, 0, 1, 1);

    GtkWidget *create_project_button = gtk_button_new_with_label("Create Project");
    g_signal_connect(create_project_button, "clicked", G_CALLBACK(on_create_project_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), create_project_button, 0, 1, 1, 1);

    GtkWidget *create_task_button = gtk_button_new_with_label("Create Task");
    g_signal_connect(create_task_button, "clicked", G_CALLBACK(on_create_task_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), create_task_button, 1, 1, 1, 1);

    GtkWidget *update_task_status_button = gtk_button_new_with_label("Update Task Status");
    g_signal_connect(update_task_status_button, "clicked", G_CALLBACK(on_update_task_status_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), update_task_status_button, 0, 2, 1, 1);

    GtkWidget *add_comment_button = gtk_button_new_with_label("Add Comment");
    g_signal_connect(add_comment_button, "clicked", G_CALLBACK(on_add_comment_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), add_comment_button, 1, 2, 1, 1);

    GtkWidget *chat_button = gtk_button_new_with_label("Chat");
    g_signal_connect(chat_button, "clicked", G_CALLBACK(on_chat_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), chat_button, 0, 3, 1, 1);

    GtkWidget *video_call_button = gtk_button_new_with_label("Video Call");
    g_signal_connect(video_call_button, "clicked", G_CALLBACK(on_video_call_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), video_call_button, 1, 3, 1, 1);

    gtk_widget_show_all(window);
    gtk_main();

    close(client_socket);
    return 0;
}