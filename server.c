#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include <sqlite3.h>
#include <gst/gst.h>

sqlite3 *db;

void log_message(const char *message) {
    FILE *log_file = fopen("server.log", "a");
    if (log_file == NULL) {
        perror("Failed to open log file");
        return;
    }
    time_t now = time(NULL);
    fprintf(log_file, "%s: %s\n", ctime(&now), message);
    fclose(log_file);
}

void handle_register(int client_socket, char *buffer) {
    char username[50], password[50], email[50];
    sscanf(buffer, "REGISTER %s %s %s", username, password, email);

    char sql[256];
    sprintf(sql, "INSERT INTO users (username, password, email) VALUES ('%s', '%s', '%s');", username, password, email);

    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        send(client_socket, "Register failed", strlen("Register failed"), 0);
    } else {
        send(client_socket, "Register successful", strlen("Register successful"), 0);
    }
}

void handle_login(int client_socket, char *buffer) {
    char username[50], password[50];
    sscanf(buffer, "LOGIN %s %s", username, password);

    char sql[256];
    sprintf(sql, "SELECT * FROM users WHERE username='%s' AND password='%s';", username, password);

    sqlite3_stmt *res;
    int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    if (rc == SQLITE_OK) {
        if (sqlite3_step(res) == SQLITE_ROW) {
            send(client_socket, "Login successful", strlen("Login successful"), 0);
        } else {
            send(client_socket, "Login failed", strlen("Login failed"), 0);
        }
    } else {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        send(client_socket, "Login failed", strlen("Login failed"), 0);
    }
    sqlite3_finalize(res);
}

void handle_create_project(int client_socket, char *buffer) {
    char project_name[100], description[256];
    int owner_id;
    sscanf(buffer, "CREATE_PROJECT %d %s %s", &owner_id, project_name, description);

    char sql[256];
    sprintf(sql, "INSERT INTO projects (name, description, owner_id) VALUES ('%s', '%s', %d);", project_name, description, owner_id);

    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        send(client_socket, "Create project failed", strlen("Create project failed"), 0);
    } else {
        send(client_socket, "Create project successful", strlen("Create project successful"), 0);
    }
}

void handle_create_task(int client_socket, char *buffer) {
    int project_id, assigned_to;
    char task_name[100], status[20];
    sscanf(buffer, "CREATE_TASK %d %d %s %s", &project_id, &assigned_to, task_name, status);

    char sql[256];
    sprintf(sql, "INSERT INTO tasks (project_id, assigned_to, name, status) VALUES (%d, %d, '%s', '%s');", project_id, assigned_to, task_name, status);

    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        send(client_socket, "Create task failed", strlen("Create task failed"), 0);
    } else {
        send(client_socket, "Create task successful", strlen("Create task successful"), 0);
    }
}

void handle_update_task_status(int client_socket, char *buffer) {
    int task_id;
    char status[20];
    sscanf(buffer, "UPDATE_TASK_STATUS %d %s", &task_id, status);

    char sql[256];
    sprintf(sql, "UPDATE tasks SET status='%s' WHERE id=%d;", status, task_id);

    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        send(client_socket, "Update task status failed", strlen("Update task status failed"), 0);
    } else {
        send(client_socket, "Update task status successful", strlen("Update task status successful"), 0);
    }
}

void handle_add_comment(int client_socket, char *buffer) {
    int task_id, user_id;
    char content[256];
    sscanf(buffer, "ADD_COMMENT %d %d %s", &task_id, &user_id, content);

    char sql[256];
    sprintf(sql, "INSERT INTO comments (task_id, user_id, content) VALUES (%d, %d, '%s');", task_id, user_id, content);

    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        send(client_socket, "Add comment failed", strlen("Add comment failed"), 0);
    } else {
        send(client_socket, "Add comment successful", strlen("Add comment successful"), 0);
    }
}

void handle_chat(int client_socket, char *buffer) {
    int project_id, user_id;
    char content[256];
    sscanf(buffer, "CHAT %d %d %s", &project_id, &user_id, content);

    char sql[256];
    sprintf(sql, "INSERT INTO messages (project_id, user_id, content) VALUES (%d, %d, '%s');", project_id, user_id, content);

    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        send(client_socket, "Chat message failed", strlen("Chat message failed"), 0);
    } else {
        send(client_socket, "Chat message successful", strlen("Chat message successful"), 0);
    }
}

void handle_video_call(int client_socket, char *buffer) {
    // Initialize GStreamer
    gst_init(NULL, NULL);

    // Create the elements
    GstElement *pipeline = gst_pipeline_new("video-call-pipeline");
    GstElement *source = gst_element_factory_make("videotestsrc", "source");
    GstElement *sink = gst_element_factory_make("autovideosink", "sink");

    if (!pipeline || !source || !sink) {
        g_printerr("Not all elements could be created.\n");
        return;
    }

    // Build the pipeline
    gst_bin_add_many(GST_BIN(pipeline), source, sink, NULL);
    if (gst_element_link(source, sink) != TRUE) {
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(pipeline);
        return;
    }

    // Start playing
    GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Unable to set the pipeline to the playing state.\n");
        gst_object_unref(pipeline);
        return;
    }

    // Wait until error or EOS
    GstBus *bus = gst_element_get_bus(pipeline);
    GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    // Parse message
    if (msg != NULL) {
        GError *err;
        gchar *debug_info;

        switch (GST_MESSAGE_TYPE(msg)) {
            case GST_MESSAGE_ERROR:
                gst_message_parse_error(msg, &err, &debug_info);
                g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
                g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
                g_clear_error(&err);
                g_free(debug_info);
                break;
            case GST_MESSAGE_EOS:
                g_print("End-Of-Stream reached.\n");
                break;
            default:
                // We should not reach here because we only asked for ERRORs and EOS
                g_printerr("Unexpected message received.\n");
                break;
        }
        gst_message_unref(msg);
    }

    // Free resources
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    send(client_socket, "Video call ended", strlen("Video call ended"), 0);
}

void *handle_client(void *arg) {
    int client_socket = *(int *)arg;
    char buffer[1024] = {0};
    int valread;

    // Req from client
    valread = read(client_socket, buffer, 1024);
    log_message(buffer);

    if (strncmp(buffer, "REGISTER", 8) == 0) {
        handle_register(client_socket, buffer);
    } else if (strncmp(buffer, "LOGIN", 5) == 0) {
        handle_login(client_socket, buffer);
    } else if (strncmp(buffer, "CREATE_PROJECT", 14) == 0) {
        handle_create_project(client_socket, buffer);
    } else if (strncmp(buffer, "CREATE_TASK", 11) == 0) {
        handle_create_task(client_socket, buffer);
    } else if (strncmp(buffer, "UPDATE_TASK_STATUS", 18) == 0) {
        handle_update_task_status(client_socket, buffer);
    } else if (strncmp(buffer, "ADD_COMMENT", 11) == 0) {
        handle_add_comment(client_socket, buffer);
    } else if (strncmp(buffer, "CHAT", 4) == 0) {
        handle_chat(client_socket, buffer);
    } else if (strncmp(buffer, "VIDEO_CALL", 10) == 0) {
        handle_video_call(client_socket, buffer);
    }

    send(client_socket, buffer, strlen(buffer), 0);
    log_message(buffer);

    close(client_socket);
    return NULL;
}

int main() {
    int rc = sqlite3_open("project_management.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_size;
    pthread_t tid;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    while (1) {
        client_addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_size);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }
        pthread_create(&tid, NULL, handle_client, &client_socket);
        pthread_detach(tid);
    }

    close(server_socket);
    sqlite3_close(db);
    return 0;
}