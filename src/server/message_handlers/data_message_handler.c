#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "data_message_handler.h"
#include "task_dao.h"
#include "logger.h"
#include "socket_handler.h"  // Để sử dụng send_data
#include <sys/stat.h>
extern sqlite3 *db;
// task_name, description, project_id, assignee_id
void handle_create_task(int client_fd, const char *task_name, const char *description, int project_id, int assignee_id) {
    int rc = insert_task(db, task_name, description, project_id, assignee_id);
    if (rc == SQLITE_OK) {
        send_data(client_fd, "TASK_CREATED");
        log_info("Task '%s' created successfully for project %d and assigned to user %d", task_name, project_id, assignee_id);
    } else {
        send_data(client_fd, "TASK_CREATION_FAILED");
        log_error("Failed to create task '%s' for project %d", task_name, project_id);
    }
}

// 

// Hàm xử lý yêu cầu lấy danh sách task
void handle_get_tasks(int client_fd, int project_id) {
    TaskArray tasks = get_tasks_by_project(db, project_id);

    if (tasks.count == 0) {
        send_data(client_fd, "NO_TASKS");
        log_info("Project %d has no tasks", project_id);
        return;
    }

    char response[4096]; // Tăng kích thước bộ đệm nếu cần
    response[0] = '\0'; // Đảm bảo chuỗi rỗng ban đầu

    for (int i = 0; i < tasks.count; i++) {
        char task_line[512];
        snprintf(task_line, sizeof(task_line), 
                 "TASK_ID: %d, NAME: %s, DESCRIPTION: %s, STATUS: %s, ASSIGNEE_ID: %d\n",
                 tasks.tasks[i].task_id, tasks.tasks[i].name, tasks.tasks[i].description, 
                 tasks.tasks[i].status, tasks.tasks[i].assignee_id);
        strncat(response, task_line, sizeof(response) - strlen(response) - 1);
    }

    printf("%s", response);
    send_data(client_fd, response); // Gửi toàn bộ dữ liệu một lần
    free_task_array(&tasks); // Giải phóng bộ nhớ
    log_info("Sent task list for project %d to client_fd %d", project_id, client_fd);
}

void handle_get_comments(int client_fd, int task_id) {
    char response[4096]; // Kích thước bộ đệm
    response[0] = '\0';  // Đảm bảo chuỗi rỗng ban đầu

    // Lấy danh sách comment
    CommentArray comments = get_comments_by_task(db, task_id);
    if (comments.count > 0) {
        strncat(response, "COMMENTS:\n", sizeof(response) - strlen(response) - 1);
        for (int i = 0; i < comments.count; i++) {
            char comment_line[512];
            snprintf(comment_line, sizeof(comment_line),
                     "USER_ID: %d, CONTENT: %s\n",
                     comments.comments[i].user_id, comments.comments[i].content);
            strncat(response, comment_line, sizeof(response) - strlen(response) - 1);
        }
    } else {
        strncat(response, "COMMENTS: NONE\n", sizeof(response) - strlen(response) - 1);
    }

    // Gửi dữ liệu đến client
    send_data(client_fd, response);
    log_info("Sent comments for task_id %d to client_fd %d", task_id, client_fd);

    // Giải phóng bộ nhớ
    free_comment_array(&comments);
}

void handle_get_attachments(int client_fd, int task_id) {
    char response[4096]; // Kích thước bộ đệm
    response[0] = '\0';  // Đảm bảo chuỗi rỗng ban đầu

    // Lấy danh sách file đính kèm
    AttachmentArray attachments = get_attachments_by_task(db, task_id);
    if (attachments.count > 0) {
        strncat(response, "ATTACHMENTS:\n", sizeof(response) - strlen(response) - 1);
        for (int i = 0; i < attachments.count; i++) {
            char attachment_line[512];
            snprintf(attachment_line, sizeof(attachment_line),
                     "FILENAME: %s, FILE_PATH: %s, UPLOADED_BY: %d\n",
                     attachments.attachments[i].filename, attachments.attachments[i].file_path,
                     attachments.attachments[i].uploaded_by);
            strncat(response, attachment_line, sizeof(response) - strlen(response) - 1);
        }
        printf("response: %s", response);
    } else {
        strncat(response, "ATTACHMENTS: NONE\n", sizeof(response) - strlen(response) - 1);
    }

    // Gửi dữ liệu đến client
    send_data(client_fd, response);
    log_info("Sent attachments for task_id %d to client_fd %d", task_id, client_fd);

    // Giải phóng bộ nhớ
    free_attachment_array(&attachments);
}


void handle_task_update(int client_fd, int task_id, const char* status) {
    if (update_task_status(db, task_id, status) == SQLITE_OK) {
        send_data(client_fd, "TASK_STATUS_UPDATED");
        log_info("Task %d updated to status: %s", task_id, status);
    } else {
        send_data(client_fd, "TASK_STATUS_UPDATE_FAILED");
        log_error("Failed to update status for task %d", task_id);
    }
}

void handle_add_comment(int client_fd, int task_id, int user_id, const char* comment) {
    if (add_comment_to_task(db, task_id, user_id, comment) == SQLITE_OK) {
        send_data(client_fd, "COMMENT_ADDED");
        log_info("Comment added to task %d by user %d", task_id, user_id);
    } else {
        send_data(client_fd, "COMMENT_FAILED");
        log_error("Failed to add comment to task %d", task_id);
    }
}


#define BUFFER_SIZE 1024

// Đường dẫn thư mục uploads
#define UPLOADS_DIR "/home/parallels/Desktop/Work-managment/Work_management/src/server/database/uploads"

// void handle_file_attachment(int client_fd,int userid) {
//     char buffer[BUFFER_SIZE];
//     char filename[256];
//     char file_path[512];
//     int task_id;

//     // Nhận metadata từ client
//     memset(buffer, 0, BUFFER_SIZE);
//     if (recv(client_fd, buffer, BUFFER_SIZE, 0) <= 0) {
//         log_error("Failed to receive metadata from client.");
//         send(client_fd, "FILE_ATTACHMENT_FAILED", strlen("FILE_ATTACHMENT_FAILED"), 0);
//         return;
//     }

//     // Phân tích metadata
//     if (sscanf(buffer, "ADD_ATTACHMENT %d %255s", &task_id, filename) != 2) {
//         log_error("Invalid metadata format received: %s", buffer);
//         send(client_fd, "FILE_ATTACHMENT_FAILED", strlen("FILE_ATTACHMENT_FAILED"), 0);
//         return;
//     }

//     // Kiểm tra hoặc tạo thư mục uploads
//     struct stat st = {0};
//     if (stat(UPLOADS_DIR, &st) == -1) {
//         if (mkdir(UPLOADS_DIR, 0700) != 0) {
//             log_error("Failed to create uploads directory: %s", UPLOADS_DIR);
//             send(client_fd, "FILE_ATTACHMENT_FAILED", strlen("FILE_ATTACHMENT_FAILED"), 0);
//             return;
//         }
//     }

//     // Xây dựng đường dẫn file để lưu
//     snprintf(file_path, sizeof(file_path), "%s/%s", UPLOADS_DIR, filename);

//     // Mở file để ghi dữ liệu
//     FILE *file = fopen(file_path, "wb");
//     if (file == NULL) {
//         log_error("Failed to open file for writing: %s", file_path);
//         send(client_fd, "FILE_ATTACHMENT_FAILED", strlen("FILE_ATTACHMENT_FAILED"), 0);
//         return;
//     }

//     // Nhận dữ liệu file từ client
//     while (1) {
//         memset(buffer, 0, BUFFER_SIZE);
//         ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
//         if (bytes_received <= 0) break; // Kết thúc nhận file hoặc gặp lỗi
//         fwrite(buffer, 1, bytes_received, file);
//     }
//     fclose(file);

//     // Cập nhật cơ sở dữ liệu
//     if (add_attachment_to_task(db,task_id, filename, file_path, userid) == SQLITE_OK) {
//         send(client_fd, "ATTACHMENT_ADDED", strlen("ATTACHMENT_ADDED"), 0);
//         log_info("File %s attached to task %d successfully.", filename, task_id);
//     } else {
//         send(client_fd, "FILE_ATTACHMENT_FAILED", strlen("FILE_ATTACHMENT_FAILED"), 0);
//         log_error("Failed to attach file %s to task %d.", filename, task_id);
//     }
// }
void handle_file_attachment(int client_fd, int userid,int task_id, char *filename) {
    char buffer[BUFFER_SIZE];
    printf("tassddk: %d\n", task_id);
    char file_path[512];
    FILE *file = NULL;

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            log_error("Failed to receive data from client.");
            if (file) fclose(file);
            return;
        }

        // Kiểm tra tín hiệu kết thúc
        if (strncmp(buffer, "DATA ADD_ATTACHMENT END", 23) == 0) {
            log_info("File transfer completed.");
            if (file) fclose(file);
            break;
        }

        // Xử lý metadata
        if (strncmp(buffer, "DATA ADD_ATTACHMENT", 19) == 0) {
            if (!file) {
                // Lấy metadata từ gói đầu tiên
                // if (sscanf(buffer, "DATA ADD_ATTACHMENT %d %255s", &task_id, filename) != 2) {
                //     log_error("Failed to parse metadata.");
                //     return;
                // }

                // Tạo đường dẫn file
                snprintf(file_path, sizeof(file_path), "%s/%s", UPLOADS_DIR, filename);

                // Mở file để ghi nội dung
                file = fopen(file_path, "wb");
                if (file == NULL) {
                    log_error("Failed to open file for writing: %s", file_path);
                    return;
                }

                log_info("Receiving file: %s", file_path);
            } else {
                // Ghi nội dung file từ gói tiếp theo
                char *content = buffer + 20; // Bỏ tiền tố "DATA ADD_ATTACHMENT "
                fwrite(content, 1, bytes_received - 20, file);
            }
        }
    }

    // Thêm file vào cơ sở dữ liệu
    if (add_attachment_to_task(db, task_id, filename, file_path, userid) == SQLITE_OK) {
        send(client_fd, "ATTACHMENT_ADDED", strlen("ATTACHMENT_ADDED"), 0);
        log_info("File %s attached to task %d successfully.", filename, task_id);
    } else {
        send(client_fd, "FILE_ATTACHMENT_FAILED", strlen("FILE_ATTACHMENT_FAILED"), 0);
        log_error("Failed to attach file %s to task %d.", filename, task_id);
    }
}
void handle_data_message(int client_fd, int userid, const char* message) {
    char action[50];
    int task_id, user_id, uploaded_by;
    int project_id;
    char status[50], comment[500], filename[100], file_path[200];
    printf("message: %s\n", message);
    // Phân tích hành động chính từ thông điệp
    if (sscanf(message, "%49s", action) != 1) {
        log_error("Failed to parse action from message: %s", message);
        send_data(client_fd, "INVALID_MESSAGE_FORMAT");
        return;
    }
    printf("action: %s\n", action);
    if (strcmp(action,"GET_TASK")==0){
        printf("Ok get task\n");
        printf("message: %s\n", message);
        if (sscanf(message, "GET_TASK %d",&project_id) == 1) {
            printf("recieved get_taks %d\n", project_id);
            handle_get_tasks(client_fd, project_id);
        } else {
            send_data(client_fd, "INVALID_GET_TASK_FORMAT");
            log_error("Invalid GET_TASK format: %s", message);
        }
    }else if (strcmp(action,"TASK_UPDATE")==0){
        if (sscanf(message,"TASK_UPDATE %d %49s", &task_id, status) == 2) {
            handle_task_update(client_fd, task_id, status);
        } else {
            send_data(client_fd, "INVALID_TASK_UPDATE_FORMAT");
            log_error("Invalid TASK_UPDATE format: %s", message);
        }
    }

    // if (strcmp(action, "TASK_UPDATE") == 0) {
    //     // Phân tích task_id và status
    //     if (sscanf(message, "TASK_UPDATE %d %49s", &task_id, status) == 2) {
    //         handle_task_update(client_fd, task_id, status);
    //     } else {
    //         send_data(client_fd, "INVALID_TASK_UPDATE_FORMAT");
    //         log_error("Invalid TASK_UPDATE format: %s", message);
    //     }
    // } else 
    // if (strcmp(action,"GET_TASK")==0) {
    //     printf ("gettask");
        
        // if (sscanf(message, "GET_TASK %d",project_id) == 2) {
        //     printf("recieved get_taks %d\n", project_id);
        //     handle_get_tasks(client_fd, project_id);
        // } else {
        //     send_data(client_fd, "INVALID_GET_TASK_FORMAT");
        //     log_error("Invalid GET_TASK format: %s", message);
        // }
    // }

    else if (strcmp(action, "CREATE_TASK") == 0) {
        char task_name[100], description[255];
        int project_id, assignee_id;
        printf("create_task 1");
        sscanf(message + 12, "%s %s %d %d", task_name, description,&project_id, &assignee_id);
        printf("create_task 2");
        
        handle_create_task(client_fd, task_name,description, project_id, assignee_id) ;
   } 
    
    else if (strcmp(action, "ADD_COMMENT") == 0) {
        // Phân tích task_id, user_id, và comment
        if (sscanf(message, "ADD_COMMENT %d %s[^\n]", &task_id, comment) == 2) {
            
            handle_add_comment(client_fd, task_id, userid, comment);
        } else {
            send_data(client_fd, "INVALID_ADD_COMMENT_FORMAT");
            log_error("Invalid ADD_COMMENT format: %s", message);
        }
    } else if (strcmp(action, "GET_COMMENTS")==0){
        if (sscanf(message, "GET_COMMENTS %d[^\n]", &task_id) == 1) {
            handle_get_comments(client_fd, task_id);
        } else {
            send_data(client_fd, "INVALID_GET_COMMENTS_FORMAT");
            log_error("Invalid GET_COMMENTS format: %s", message);
        }
    }

     else if (strcmp(action, "GET_ATTACHMENTS") == 0) {
        // Phân tích task_id
        if (sscanf(message, "GET_ATTACHMENTS %d", &task_id) == 1) {
            handle_get_attachments(client_fd, task_id);
        } else {
            send_data(client_fd, "INVALID_GET_ATTACHMENTS_FORMAT");
            log_error("Invalid GET_ATTACHMENTS format: %s", message);
        }
    }
    
    
    else if (strcmp(action, "ADD_ATTACHMENT") == 0) {
        // Phân tích task_id, filename, file_path và uploaded_by
        if (sscanf(message, "ADD_ATTACHMENT %d %99s", &task_id, filename) == 2) {
            printf(" taskidi %d",task_id);
            handle_file_attachment(client_fd,user_id, task_id, filename);

        } else {
            send_data(client_fd, "INVALID_FILE_ATTACHMENT_FORMAT");
            log_error("Invalid FILE_ATTACHMENT format: %s", message);
        }
    } else {
        send_data(client_fd, "UNKNOWN_ACTION");
        log_error("Unknown message type: %s", message);
    }
}

