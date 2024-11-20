#ifndef USER_DAO_H
#define USER_DAO_H

#include <sqlite3.h>

int insert_user(sqlite3 *db, const char *username, const char *password_hash, const char *email);
int login_user(sqlite3 *db, const char *username, const char *password_hash);
int user_exists(sqlite3 *db, const char *username);

#endif 
