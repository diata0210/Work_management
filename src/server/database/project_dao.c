#include <stdio.h>
#include <sqlite3.h>
#include "project_dao.h"

// Thêm dự án mới vào bảng `projects`
int insert_project(sqlite3 *db, const char *name, const char *description, int created_by) {
    char *err_msg = 0;
    char sql[256];
    snprintf(sql, sizeof(sql), "INSERT INTO projects (name, description, created_by, status) VALUES ('%s', '%s', %d, 'ongoing');", 
             name, description, created_by);

    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return rc;
    }

    printf("Project added successfully\n");
    return SQLITE_OK;
}

// Mời thành viên vào dự án và chỉ định vai trò cho họ
int add_member_to_project(sqlite3 *db, int project_id, int user_id, const char *role) {
    char *err_msg = 0;
    char sql[256];
    snprintf(sql, sizeof(sql), "INSERT INTO project_members (project_id, user_id, role) VALUES (%d, %d, '%s');", project_id, user_id, role);

    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return rc;
    }

    printf("Member added to project successfully\n");
    return SQLITE_OK;
}

// Lấy tất cả dự án của một user
int get_projects_by_user(sqlite3 *db, int user_id) {
    const char *sql = "SELECT projects.project_id, projects.name FROM projects "
                      "JOIN project_members ON projects.project_id = project_members.project_id "
                      "WHERE project_members.user_id = ?";
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to fetch projects: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_int(stmt, 1, user_id);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int project_id = sqlite3_column_int(stmt, 0);
        const char *name = (const char *)sqlite3_column_text(stmt, 1);
        printf("Project ID: %d, Name: %s\n", project_id, name);
    }

    sqlite3_finalize(stmt);
    return SQLITE_OK;
}
