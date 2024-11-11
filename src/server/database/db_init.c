#include <stdio.h>
#include <sqlite3.h>
#include "config.h" 
#include "db_init.h" 

sqlite3 *db = NULL;

int initialize_database(const char *db_path) {
    int rc = sqlite3_open(DB_PATH, &db); 

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    return rc;
}

void close_database() {
    if (db) {
        sqlite3_close(db);
        db = NULL;
    }
}