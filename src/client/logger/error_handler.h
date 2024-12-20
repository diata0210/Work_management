#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <sqlite3.h>

// Hàm xử lý lỗi chung - ghi lỗi vào console và dừng chương trình nếu cần
void handle_error(const char *message, int is_fatal);

// Hàm xử lý lỗi SQL - ghi lỗi SQL và dọn dẹp tài nguyên nếu cần
void handle_sql_error(sqlite3 *db, const char *message);

// Hàm xử lý lỗi socket - ghi lỗi socket và đóng kết nối nếu cần
void handle_socket_error(int socket_fd, const char *message);

#endif // ERROR_HANDLER_H
