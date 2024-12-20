#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "control_message_handler.h"
#include "socket.h"  // Giả sử bạn có một hàm send_data()
#include "logger.h"

// Hàm gửi thông điệp đăng nhập tới server
void send_login(int client_fd, const char* username, const char* password) {
    char login_message[512];

    // Tạo thông điệp đăng nhập theo định dạng: LOGIN <username> <password>
    snprintf(login_message, sizeof(login_message), "CONTROL LOGIN %s %s", username, password);

    // Gửi thông điệp đăng nhập tới server
    if (send(client_fd, login_message, strlen(login_message), 0) == -1) {
        perror("send_login() failed");
    } else {
        printf("Login message sent to server: %s\n", login_message);
    }
}

// Hàm gửi thông điệp đăng ký tới server
void send_register(int client_fd, const char* username, const char* password) {
    char register_message[512];

    // Tạo thông điệp đăng ký theo định dạng: REGISTER <username> <password>
    snprintf(register_message, sizeof(register_message), "CONTROL REGISTER %s %s", username, password);

    // Gửi thông điệp đăng ký tới server
    if (send(client_fd, register_message, strlen(register_message), 0) == -1) {
        perror("send_register() failed");
    } else {
        printf("Register message sent to server: %s\n", register_message);
    }
}

// Hàm gửi thông điệp tạo dự án tới server
void send_create_project(int client_fd, const char* project_name, const char* description, int created_by) {
    char create_project_message[512];

    // Tạo thông điệp tạo dự án theo định dạng: CREATE_PROJECT <project_name> <description> <created_by>
    snprintf(create_project_message, sizeof(create_project_message), 
             "CONTROL CREATE_PROJECT %s %s %d", project_name, description, created_by);

    // Gửi thông điệp tạo dự án tới server
    if (send(client_fd, create_project_message, strlen(create_project_message), 0) == -1) {
        perror("send_create_project() failed");
    } else {
        printf("Create project message sent to server: %s\n", create_project_message);
    }
}

// Hàm gửi thông điệp mời thành viên vào dự án
void send_add_member(int client_fd, int project_id, int user_id, const char *role) {
    char add_member_message[512];

    // Tạo thông điệp mời thành viên vào dự án theo định dạng: ADD_MEMBER <project_id> <user_id> <role>
    snprintf(add_member_message, sizeof(add_member_message), "CONTROL ADD_MEMBER %d %d %s", project_id, user_id, role);

    // Gửi thông điệp mời thành viên tới server
    if (send(client_fd, add_member_message, strlen(add_member_message), 0) == -1) {
        perror("send_add_member() failed");
    } else {
        printf("Add member message sent to server: %s\n", add_member_message);
    }
}

// Hàm xử lý thông điệp control từ server
void handle_control_message(int client_fd, const char* message) {
    char command[20];
    sscanf(message, "%s", command);

    if (strcmp(command, "LOGIN") == 0) {
        char username[50], password[50];
        sscanf(message + 6, "%s %s", username, password); // Bỏ qua "LOGIN "
        send_login(client_fd, username, password);
    } else if (strcmp(command, "REGISTER") == 0) {
        char username[50], password[50];
        sscanf(message + 9, "%s %s", username, password); // Bỏ qua "REGISTER "
        send_register(client_fd, username, password);
    } else if (strcmp(command, "CREATE_PROJECT") == 0) {
        char project_name[100], description[255];
        int created_by;
        sscanf(message + 15, "%s %s %d", project_name, description, &created_by); // Bỏ qua "CREATE_PROJECT "
        send_create_project(client_fd, project_name, description, created_by);
    } else if (strcmp(command, "ADD_MEMBER") == 0) {
        int project_id, user_id;
        char role[20];
        sscanf(message + 11, "%d %d %s", &project_id, &user_id, role); // Bỏ qua "ADD_MEMBER "
        send_add_member(client_fd, project_id, user_id, role);
    } else {
        log_error("Unknown control message: %s", message);
    }
}
