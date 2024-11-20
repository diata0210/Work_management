#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "socket_handler.h"
#include "config.h"
#include "logger.h"

void sigchld_handler(int sig) {
    (void)sig;  
    while (waitpid(-1, NULL, WNOHANG) > 0); 
}

int main() {
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART; 
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    int server_fd = setup_server_socket(SERVER_PORT);
    if (server_fd < 0) {
        log_error("Failed to set up server socket");
        return 1;
    }

    log_info("Server is running...");

    while (1) {
        int client_fd = accept_client_connection(server_fd);
        
        if (client_fd < 0) {
            log_error("Failed to accept client connection");
            continue;
        }

        pid_t pid = fork();
        if (pid == 0) {  
            close(server_fd);  

            // Nhận thông điệp từ client
            char buffer[MAX_BUFFER];
            int bytes_received = receive_message(client_fd, buffer, MAX_BUFFER);
            if (bytes_received > 0) {
                // Gọi hàm handle_client_message với client_fd và thông điệp nhận được
                handle_client_message(client_fd, buffer);
            } else {
                log_error("Failed to receive message from client");
            }

            close_connection(client_fd);
            exit(0); 
        } else if (pid > 0) { 
            close(client_fd); 
        } else {
            log_error("Failed to fork process for client");
            close_connection(client_fd);
        }
    }

    close(server_fd); 
    return 0;
}
