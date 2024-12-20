#ifndef PROJECT_DAO_H
#define PROJECT_DAO_H

#include <sqlite3.h>

// Định nghĩa cấu trúc Project
typedef struct {
    int project_id;
    char name[256];
    char description[512];
} Project;

// Cấu trúc mảng Project
typedef struct {
    Project *projects;
    int count;
} ProjectArray;

// Các hàm DAO cho bảng Project
int insert_project(sqlite3 *db, const char *name, const char *description, int created_by);
int add_member_to_project(sqlite3 *db, int project_id, int user_id, const char *role);
ProjectArray get_projects_by_user(sqlite3 *db, int user_id);
void free_project_array(ProjectArray *project_array);

#endif // PROJECT_DAO_H
