// src/client/main.c

#include <gtk/gtk.h>
#include <glib.h>
#include "client.h"
#include "auth.h"
#include "ui/main_window.h"

#define DEFAULT_HOST "localhost"
#define DEFAULT_PORT 8080

static void cleanup_callback(GtkWidget *widget, gpointer data) {
    Client *client = (Client*)data;
    client_cleanup(client);
    gtk_main_quit();
}

int main(int argc, char *argv[]) {
    // Khởi tạo GTK
    gtk_init(&argc, &argv);

    // Khởi tạo client
    Client *client = client_init();
    if (!client) {
        g_print("Failed to initialize client\n");
        return 1;
    }

    // Kết nối tới server
    if (client_connect(client, DEFAULT_HOST, DEFAULT_PORT) < 0) {
        g_print("Failed to connect to server\n");
        client_cleanup(client);
        return 1;
    }

    // Tạo cửa sổ chính
    create_main_window(client);
    
    // Kết nối signal cleanup
    g_signal_connect(client->main_window, "destroy", 
                    G_CALLBACK(cleanup_callback), client);

    // Load credentials đã lưu (nếu có)
    AuthInfo *saved_auth = auth_load_credentials();
    if (saved_auth) {
        if (auth_login(client, saved_auth->username, saved_auth->password) == 0) {
            g_print("Auto login successful\n");
        }
        g_free(saved_auth);
    }

    // Chạy main loop
    gtk_main();

    return 0;
}
