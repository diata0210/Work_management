#ifndef PROJECT_DAO_H
#define PROJECT_DAO_H

#include <sqlite3.h>

int insert_project(sqlite3 *db, const char *name, const char *description, int created_by);
int add_member_to_project(sqlite3 *db, int project_id, int user_id, const char *role);
int get_projects_by_user(sqlite3 *db, int user_id);

#endif // PROJECT_DAO_H
