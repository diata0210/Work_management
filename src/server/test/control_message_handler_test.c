#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../../client/include/socket.h"  // Bao gồm file socket.h
#include "../../client/include/socket.h"  // Bao gồm file cấu hình, nếu cần

// Hàm để gửi và nhận thông điệp, kiểm thử kết nối tới server
int send_message_and_receive_response(int client_fd, const char *message) {
    if (send_message(client_fd, message) == -1) {
        printf("Failed to send message: %s\n", message);
        return -1;
    }

    char buffer[1024];
    if (receive_message(client_fd, buffer, sizeof(buffer)) == -1) {
        printf("Failed to receive message\n");
        return -1;
    }

    printf("Received message: %s\n", buffer);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <port>\n", argv[0]);
        return -1;
    }

    const char *server_ip = argv[1];
    int port = atoi(argv[2]);
    
    // Kết nối tới server
    int client_fd = connect_to_server(server_ip, port);
    if (client_fd == -1) {
        return -1; // Nếu không kết nối được thì kết thúc
    }

    // Kiểm thử các chức năng gửi và nhận thông điệp
    if (send_message_and_receive_response(client_fd, "LOGIN user1 password1") == -1) {
        printf("Test failed for LOGIN\n");
    }

    if (send_message_and_receive_response(client_fd, "REGISTER newuser newpass") == -1) {
        printf("Test failed for REGISTER\n");
    }

    if (send_message_and_receive_response(client_fd, "CREATE_PROJECT MyProject A new project 1") == -1) {
        printf("Test failed for CREATE_PROJECT\n");
    }

    if (send_message_and_receive_response(client_fd, "ADD_MEMBER 1 2 admin") == -1) {
        printf("Test failed for ADD_MEMBER\n");
    }

    // Đóng kết nối
    close(client_fd);
    return 0;
}
