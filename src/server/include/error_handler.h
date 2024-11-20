#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <sqlite3.h>

// Xử lý lỗi chung với tùy chọn dừng chương trình nếu là lỗi nghiêm trọng
void handle_error(const char *message, int is_fatal);

// Xử lý lỗi SQL với thông báo cụ thể và lỗi từ SQLite
void handle_sql_error(sqlite3 *db, const char *message);

// Xử lý lỗi socket với thông báo cụ thể và tùy chọn đóng kết nối
void handle_socket_error(int socket_fd, const char *message);

#endif // ERROR_HANDLER_H
