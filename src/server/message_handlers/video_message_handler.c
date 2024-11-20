#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "video_message_handler.h"
#include "logger.h"

// Giả định cấu trúc lưu các cặp kết nối giữa các client
typedef struct {
    int client_fd;
    int peer_fd;
} PeerConnection;

// Mảng tĩnh cho các cặp kết nối (chỉ là một ví dụ, có thể thay bằng hash map)
#define MAX_CONNECTIONS 100
PeerConnection peer_connections[MAX_CONNECTIONS];
int connection_count = 0;

// Tìm client đối tác cho client hiện tại
int find_peer_for_client(int client_fd) {
    for (int i = 0; i < connection_count; i++) {
        if (peer_connections[i].client_fd == client_fd) {
            return peer_connections[i].peer_fd;
        } else if (peer_connections[i].peer_fd == client_fd) {
            return peer_connections[i].client_fd;
        }
    }
    return -1; // Không tìm thấy đối tác
}

// Thêm một cặp kết nối mới
void add_peer_connection(int client_fd, int peer_fd) {
    if (connection_count < MAX_CONNECTIONS) {
        peer_connections[connection_count].client_fd = client_fd;
        peer_connections[connection_count].peer_fd = peer_fd;
        connection_count++;
        log_info("Added peer connection: %d <-> %d", client_fd, peer_fd);
    } else {
        log_error("Max peer connections reached. Cannot add more connections.");
    }
}

// Gửi thông điệp đến client đối tác
void send_to_peer(int peer_fd, const char* message) {
    if (send(peer_fd, message, strlen(message), 0) == -1) {
        log_error("Failed to send message to peer with fd %d", peer_fd);
    }
}

// Xử lý tín hiệu OFFER từ client
void handle_offer(int client_fd, const char* offer) {
    log_info("Received OFFER from client %d", client_fd);
    int client_peer_fd = find_peer_for_client(client_fd);
    if (client_peer_fd != -1) {
        send_to_peer(client_peer_fd, offer);  // Chuyển tiếp OFFER đến đối tác
    } else {
        log_error("No peer found for client %d", client_fd);
    }
}

// Xử lý tín hiệu ANSWER từ client
void handle_answer(int client_fd, const char* answer) {
    log_info("Received ANSWER from client %d", client_fd);
    int client_peer_fd = find_peer_for_client(client_fd);
    if (client_peer_fd != -1) {
        send_to_peer(client_peer_fd, answer);  // Chuyển tiếp ANSWER
    } else {
        log_error("No peer found for client %d", client_fd);
    }
}

// Xử lý tín hiệu ICE Candidate từ client
void handle_ice_candidate(int client_fd, const char* candidate) {
    log_info("Received ICE candidate from client %d", client_fd);
    int client_peer_fd = find_peer_for_client(client_fd);
    if (client_peer_fd != -1) {
        send_to_peer(client_peer_fd, candidate);  // Chuyển tiếp ICE Candidate
    } else {
        log_error("No peer found for client %d", client_fd);
    }
}

// Hàm phân loại và xử lý thông điệp video (OFFER, ANSWER, ICE)
void handle_video_message(int client_fd, const char* message) {
    if (strncmp(message, "OFFER", 5) == 0) {
        handle_offer(client_fd, message + 6);
    } else if (strncmp(message, "ANSWER", 6) == 0) {
        handle_answer(client_fd, message + 7);
    } else if (strncmp(message, "ICE", 3) == 0) {
        handle_ice_candidate(client_fd, message + 4);
    } else {
        log_error("Unknown video message from client %d: %s", client_fd, message);
    }
}
