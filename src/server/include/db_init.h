#ifndef DB_INIT_H
#define DB_INIT_H
#include <sqlite3.h> 

int initialize_database(const char *db_path);
void close_database(); 
#endif 
