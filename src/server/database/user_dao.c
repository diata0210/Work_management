#include <stdio.h>
#include <sqlite3.h>
#include "user_dao.h"

// Thêm người dùng vào bảng `users` - Tạo tài khoản
int insert_user(sqlite3 *db, const char *username, const char *password_hash, const char *email) {
    char *err_msg = 0;
    char sql[256];
    snprintf(sql, sizeof(sql), "INSERT INTO users (username, password_hash, email, status) VALUES ('%s', '%s', '%s', 'active');", 
             username, password_hash, email);

    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return rc;
    }

    printf("User added successfully\n");
    return SQLITE_OK;
}

// Kiểm tra thông tin đăng nhập - Xác thực người dùng
int login_user(sqlite3 *db, const char *username, const char *password_hash) {
    const char *sql = "SELECT user_id FROM users WHERE username = ? AND password_hash = ?";
    sqlite3_stmt *stmt;
    int userid = -1;

    // Chuẩn bị câu truy vấn
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1; // Trả về -1 nếu có lỗi
    }

    // Gắn giá trị cho các tham số truy vấn
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password_hash, -1, SQLITE_STATIC);

    // Thực thi truy vấn và kiểm tra kết quả
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        userid = sqlite3_column_int(stmt, 0); // Lấy cột đầu tiên (id)
    }

    // Giải phóng tài nguyên
    sqlite3_finalize(stmt);

    return userid; // Trả về userid nếu thành công, hoặc -1 nếu thất bại
}


// Kiểm tra xem người dùng có tồn tại không
int user_exists(sqlite3 *db, const char *username) {
    char sql[256];
    sqlite3_stmt *stmt;

    snprintf(sql, sizeof(sql), "SELECT 1 FROM users WHERE username = '%s';", username);

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    int exists = (sqlite3_step(stmt) == SQLITE_ROW);  // Có dòng nào khớp thì tồn tại
    sqlite3_finalize(stmt);

    return exists ? 1 : 0;
}
