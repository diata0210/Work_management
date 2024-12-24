#ifndef CHAT_CLIENT_HANDLER_H
#define CHAT_CLIENT_HANDLER_H

#include <gtk/gtk.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFF_SIZE 1024

// Global variables
extern int client_socket;
extern char buff[BUFF_SIZE];

// Function declarations
void *receive_handler(void *arg);
void send_message(const char *message, int project_id);
void on_send_message_clicked(GtkButton *button, gpointer user_data);
GtkWidget *create_chat_window(int project_id);

#endif // CHAT_CLIENT_HANDLER_H
