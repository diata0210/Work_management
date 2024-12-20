#include <stdio.h>
#include <stdlib.h>
#include "error_handler.h"

// Hàm xử lý lỗi chung - ghi lỗi vào console và dừng chương trình nếu cần
void handle_error(const char *message, int is_fatal) {
    fprintf(stderr, "ERROR: %s\n", message);
    if (is_fatal) {
        exit(EXIT_FAILURE); // Dừng chương trình nếu lỗi là nghiêm trọng
    }
}


// Hàm xử lý lỗi socket - ghi lỗi socket và đóng kết nối nếu cần
void handle_socket_error(int socket_fd, const char *message) {
    fprintf(stderr, "SOCKET ERROR: %s\n", message);
    if (socket_fd >= 0) {
        close(socket_fd); // Đóng kết nối nếu có lỗi socket
    }
}
