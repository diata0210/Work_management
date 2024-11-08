
#include "database.h"
#include <stdio.h>
#include <string.h>

int init_database(sqlite3 **db) {
    int rc = sqlite3_open("project_management.db", db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(*db));
        return rc;
    }
    return SQLITE_OK;
}

void close_database(sqlite3 *db) {
    sqlite3_close(db);
}

int execute_query(sqlite3 *db, const char *sql, char *error_message) {
    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        sprintf(error_message, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return rc;
    }
    return SQLITE_OK;
}