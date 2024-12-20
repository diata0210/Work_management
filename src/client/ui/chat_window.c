#include <gtk/gtk.h>
#include "../include/client.h"
#include "../message_handlers/chat_message_handler.h"

typedef struct {
    GtkWidget *window;
    GtkWidget *messages_view;
    GtkTextBuffer *messages_buffer;
    GtkWidget *entry;
    GtkWidget *send_button;
    int chat_id;
    Client *client;
} ChatWindow;

static void on_send_clicked(GtkButton *button, gpointer user_data) {
    ChatWindow *chat = (ChatWindow*)user_data;
    GtkTextIter start, end;
    
    gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(chat->entry), &start, &end);
    char *message = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(chat->entry), 
                                           &start, &end, FALSE);
    
    if (strlen(message) > 0) {
        send_chat_message(chat->client->socket_fd, message);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(chat->entry), "", -1);
    }
    
    g_free(message);
}

void add_message_to_chat(ChatWindow *chat, const char *sender, 
                        const char *message, const char *timestamp) {
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(chat->messages_buffer, &end);
    
    char *formatted_message = g_strdup_printf("[%s] %s: %s\n", 
                                            timestamp, sender, message);
    
    gtk_text_buffer_insert(chat->messages_buffer, &end, formatted_message, -1);
    g_free(formatted_message);
    
    // Auto scroll to bottom
    GtkAdjustment *adj = gtk_scrolled_window_get_vadjustment(
        GTK_SCROLLED_WINDOW(gtk_widget_get_parent(chat->messages_view)));
    gtk_adjustment_set_value(adj, gtk_adjustment_get_upper(adj));
}

static gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    ChatWindow *chat = (ChatWindow*)user_data;
    
    if (event->keyval == GDK_KEY_Return && !(event->state & GDK_SHIFT_MASK)) {
        on_send_clicked(GTK_BUTTON(chat->send_button), chat);
        return TRUE;
    }
    
    return FALSE;
}

ChatWindow* create_chat_window(Client *client, int chat_id, const char *title) {
    ChatWindow *chat = g_new0(ChatWindow, 1);
    chat->client = client;
    chat->chat_id = chat_id;

    // Create main window
    chat->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(chat->window), title);
    gtk_window_set_default_size(GTK_WINDOW(chat->window), 400, 500);

    // Create layout
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);

    // Create messages view
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    chat->messages_view = gtk_text_view_new();
    chat->messages_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(chat->messages_view));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(chat->messages_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(chat->messages_view), GTK_WRAP_WORD_CHAR);
    gtk_container_add(GTK_CONTAINER(scroll), chat->messages_view);

    // Create input area
    GtkWidget *input_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    chat->entry = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(chat->entry), GTK_WRAP_WORD_CHAR);
    gtk_widget_set_size_request(chat->entry, -1, 50);

    chat->send_button = gtk_button_new_with_label("Send");

    gtk_box_pack_start(GTK_BOX(input_box), chat->entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(input_box), chat->send_button, FALSE, FALSE, 0);

    // Pack everything
    gtk_box_pack_start(GTK_BOX(vbox), scroll, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), input_box, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(chat->window), vbox);

    // Connect signals
    g_signal_connect(G_OBJECT(chat->send_button), "clicked",
                    G_CALLBACK(on_send_clicked), chat);
    g_signal_connect(G_OBJECT(chat->entry), "key-press-event",
                    G_CALLBACK(on_key_press), chat);

    gtk_widget_show_all(chat->window);
    return chat;
}
