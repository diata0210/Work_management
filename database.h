
#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>

int init_database(sqlite3 **db);
void close_database(sqlite3 *db);
int execute_query(sqlite3 *db, const char *sql, char *error_message);

#endif