// src/client/include/auth.h

#ifndef AUTH_H
#define AUTH_H

#include "client.h"

typedef struct {
    char *username;
    char *password;
    int user_id;
    char *token;
} AuthInfo;

// Đăng nhập
int auth_login(Client *client, const char *username, const char *password);

// Đăng ký
int auth_register(Client *client, const char *username, const char *password);

// Đăng xuất
void auth_logout(Client *client);

// Kiểm tra trạng thái đăng nhập
gboolean auth_is_logged_in(Client *client);

// Lưu thông tin xác thực
void auth_save_credentials(AuthInfo *auth_info);

// Đọc thông tin xác thực đã lưu
AuthInfo* auth_load_credentials();

#endif // AUTH_H
