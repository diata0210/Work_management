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
ProjectArray get_projects_by_user(sqlite3 *db, int user_id) {
    const char *sql = "SELECT projects.project_id, projects.name, projects.description FROM projects "
                      "JOIN project_members ON projects.project_id = project_members.project_id "
                      "WHERE project_members.user_id = ?";
    sqlite3_stmt *stmt;
    ProjectArray result = {NULL, 0};

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to fetch projects: %s\n", sqlite3_errmsg(db));
        return result;
    }

    sqlite3_bind_int(stmt, 1, user_id);

    // Đếm số lượng dự án
    int project_count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        project_count++;
    }

    // Quay lại đầu câu truy vấn
    sqlite3_reset(stmt);

    // Cấp phát bộ nhớ cho mảng dự án
    result.projects = (Project *)malloc(project_count * sizeof(Project));
    if (result.projects == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        sqlite3_finalize(stmt);
        return result;
    }
    result.count = project_count;

    // Lấy dữ liệu
    int index = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        result.projects[index].project_id = sqlite3_column_int(stmt, 0);
        const char *name = (const char *)sqlite3_column_text(stmt, 1);
        const char *description = (const char *)sqlite3_column_text(stmt, 2);

        strncpy(result.projects[index].name, name, sizeof(result.projects[index].name) - 1);
        result.projects[index].name[sizeof(result.projects[index].name) - 1] = '\0';

        strncpy(result.projects[index].description, description, sizeof(result.projects[index].description) - 1);
        result.projects[index].description[sizeof(result.projects[index].description) - 1] = '\0';

        index++;
    }

    sqlite3_finalize(stmt);
    return result;
}
void free_project_array(ProjectArray *project_array) {
    if (project_array->projects != NULL) {
        free(project_array->projects);
        project_array->projects = NULL;
        project_array->count = 0;
    }
}
