#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include "network/socket.h"
#include "ui/login_window.h"

// Hàm khởi tạo socket và kiểm tra kết nối
static bool setup_socket_connection() {
    printf("Initializing connection to server...\n");

    if (!initialize_socket()) {
        fprintf(stderr, "Failed to establish connection to server.\n");
        return false;
    }

    printf("Connection to server established successfully.\n");
    return true;
}

int main(int argc, char *argv[]) {
    // Khởi tạo GTK
    gtk_init(&argc, &argv);

    // Thiết lập kết nối socket
    if (!setup_socket_connection()) {
        fprintf(stderr, "Exiting application due to connection failure.\n");
        return EXIT_FAILURE;
    }

    // Tạo giao diện đăng nhập
    GtkWidget *login_window = create_login_window();
    if (!login_window) {
        fprintf(stderr, "Failed to create login window. Exiting...\n");
        // close_socket(); // Đảm bảo đóng socket trước khi thoát
        return EXIT_FAILURE;
    }

    // Hiển thị cửa sổ đăng nhập
    gtk_widget_show_all(login_window);

    // Bắt đầu vòng lặp GTK
    gtk_main();

    // Đóng socket khi thoát ứng dụng
    printf("Closing connection and cleaning up resources...\n");
    close_socket();

    return EXIT_SUCCESS;
}
