#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "socket_handler.h"
#include "config.h"
#include "logger.h"
#include "db_init.h"

void* handle_client_thread(void* arg);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    int server_port = atoi(argv[1]);
    if (server_port <= 1024 || server_port > 65535) {
        fprintf(stderr, "Invalid port number. Please choose a port between 1025 and 65535.\n");
        exit(1);
    }

    if (initialize_database("/home/parallels/Work_management-server/resources/database/project.db") != 0) {
        log_error("Failed to open database.");
        exit(1);
    }

    int server_fd = setup_server_socket(server_port);
    if (server_fd < 0) {
        log_error("Failed to set up server socket");
        return 1;
    }

    log_info("Server is running on port %d...", server_port);
    initialize_user_sockets();

    while (1) {
        int client_fd = accept_client_connection(server_fd);
        if (client_fd < 0) {
            log_error("Failed to accept client connection");
            continue;
        }

        // Tạo luồng để xử lý client
        pthread_t tid;
        int* pclient_fd = malloc(sizeof(int));  // Phải cấp phát động để tránh lỗi
        *pclient_fd = client_fd;

        if (pthread_create(&tid, NULL, handle_client_thread, (void*)pclient_fd) != 0) {
            log_error("Failed to create thread for client_fd %d", client_fd);
            free(pclient_fd);
            close(client_fd);
        } else {
            pthread_detach(tid);  // Tự động giải phóng luồng khi kết thúc
        }
    }

    close_database();
    close(server_fd);
    return 0;
}
