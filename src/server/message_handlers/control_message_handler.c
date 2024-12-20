#include <stdio.h>
#include <string.h>
#include "control_message_handler.h"
#include "user_dao.h"
#include "project_dao.h"
#include "logger.h"
#include "socket_handler.h"

extern sqlite3 *db;

// Hàm xử lý đăng nhập
// void handle_login(int client_fd, const char* username, const char* password) {
//     printf("%s %s",username, password);
//     int rc = login_user(db, username, password);
//     if (rc != -1 && rc != 0) {
//         send_data(client_fd, "LOGIN_SUCCESS");
//         log_info("User %s logged in successfully", username);
//     } else {
//         send_data(client_fd, "LOGIN_FAILED");
//         log_error("Login failed for user %s", username);
//     }
// }

// Hàm xử lý đăng ký tài khoản
void handle_register(int client_fd, const char* username, const char* password) {
    if (user_exists(db, username)) {
        send_data(client_fd, "REGISTER_FAILED_USER_EXISTS");
        log_error("User %s already exists", username);
    } else if (insert_user(db, username, password, "example@example.com") == SQLITE_OK) {
        send_data(client_fd, "REGISTER_SUCCESS");
        log_info("User %s registered successfully", username);
    } else {
        send_data(client_fd, "REGISTER_FAILED");
        log_error("Registration failed for user %s", username);
    }
}

// Hàm xử lý tạo dự án
void handle_create_project(int client_fd, const char* project_name, const char* description, int created_by) {
    if ((insert_project(db, project_name, description, created_by) == SQLITE_OK)) {
        send_data(client_fd, "PROJECT_CREATED");
        log_info("Project %s created successfully", project_name);
    } else {
        send_data(client_fd, "PROJECT_CREATION_FAILED");
        log_error("Failed to create project %s", project_name);
    }
}


// Mời thành viên vào dự án
void handle_add_member(int client_fd, int project_id, int user_id, const char *role) {
    if (add_member_to_project(db, project_id, user_id, role) == SQLITE_OK) {
        send_data(client_fd, "MEMBER_ADDED");
        log_info("User %d added to project %d with role %s", user_id, project_id, role);
    } else {
        send_data(client_fd, "ADD_MEMBER_FAILED");
        log_error("Failed to add user %d to project %d", user_id, project_id);
    }
}

void handle_get_projects(int client_fd, int user_id) {
    ProjectArray projects = get_projects_by_user(db, user_id);

    if (projects.count == 0) {
        send_data(client_fd, "NO_PROJECTS");
        log_info("User %d has no projects", user_id);
        return;
    }

    char response[1024];
    snprintf(response, sizeof(response), "PROJECT_LIST_COUNT %d\n", projects.count);
    send_data(client_fd, response);

    for (int i = 0; i < projects.count; i++) {
        snprintf(response, sizeof(response), "PROJECT_ID: %d, NAME: %s\n",
                 projects.projects[i].project_id, projects.projects[i].name);
        send_data(client_fd, response);
    }

    free(projects.projects); // Giải phóng bộ nhớ
    log_info("Sent project list to user %d", user_id);
}


void handle_control_message(int client_fd,int userid, const char* message) {
    char command[20];
    sscanf(message, "%s", command);

    if (strcmp(command, "LOGIN") == 0) {
        char username[50], password[50];
        sscanf(message + 6, "%s %s", username, password); // Bỏ qua "LOGIN "
        handle_login(client_fd, username, password);
    } else if (strcmp(command, "REGISTER") == 0) {
        char username[50], password[50];
        sscanf(message + 9, "%s %s", username, password); // Bỏ qua "REGISTER "
        handle_register(client_fd, username, password);
    } else if (strcmp(command, "GET_PROJECT") == 0) {
        sscanf(message + 12, "%s %s", userid); 
        handle_get_projects(client_fd,userid );
    }
     else if (strcmp(command, "CREATE_PROJECT") == 0) {
        char project_name[100], description[255];
        int created_by;
        sscanf(message + 15, "%s %s %d", project_name, description, &created_by); // Bỏ qua "CREATE_PROJECT "
        handle_create_project(client_fd, project_name, description, created_by);
    } else if (strcmp(command, "ADD_MEMBER") == 0) {
        int project_id, user_id;
        char role[20];
        sscanf(message + 11, "%d %d %s", &project_id, &user_id, role); // Bỏ qua "ADD_MEMBER "
        handle_add_member(client_fd, project_id, user_id, role);
    } else {
        log_error("Unknown control message: %s", message);
    }
}
