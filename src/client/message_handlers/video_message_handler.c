#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "video_message_handler.h"
#include "socket_handler.h"  // Giả sử bạn có một hàm send_video_message_to_peer()
#include "logger.h"

// Hàm gửi thông điệp video tới đối tác
void send_video_message_to_peer(int peer_fd, const char* message) {
    if (send(peer_fd, message, strlen(message), 0) == -1) {
        log_error("Failed to send video message to peer with fd %d", peer_fd);
    } else {
        log_info("Sent video message to peer %d: %s", peer_fd, message);
    }
}

// Hàm gửi thông điệp VIDEO OFFER tới đối tác
void send_video_offer(int client_fd, int peer_fd, const char* offer) {
    char video_offer_message[512];
    
    // Tạo thông điệp VIDEO OFFER
    snprintf(video_offer_message, sizeof(video_offer_message), "OFFER %d %s", client_fd, offer);
    
    // Gửi video OFFER đến đối tác
    send_video_message_to_peer(peer_fd, video_offer_message);
}

// Hàm gửi thông điệp VIDEO ANSWER tới đối tác
void send_video_answer(int client_fd, int peer_fd, const char* answer) {
    char video_answer_message[512];
    
    // Tạo thông điệp VIDEO ANSWER
    snprintf(video_answer_message, sizeof(video_answer_message), "ANSWER %d %s", client_fd, answer);
    
    // Gửi video ANSWER đến đối tác
    send_video_message_to_peer(peer_fd, video_answer_message);
}

// Hàm gửi thông điệp ICE Candidate tới đối tác
void send_video_ice_candidate(int client_fd, int peer_fd, const char* candidate) {
    char video_ice_message[512];
    
    // Tạo thông điệp ICE Candidate
    snprintf(video_ice_message, sizeof(video_ice_message), "ICE %d %s", client_fd, candidate);
    
    // Gửi ICE Candidate đến đối tác
    send_video_message_to_peer(peer_fd, video_ice_message);
}

// Hàm tìm kiếm đối tác cho client
int find_peer_for_client(int client_fd) {
    // Hàm tìm kiếm đối tác sẽ trả về file descriptor của đối tác.
    // Ví dụ: tìm trong danh sách kết nối hiện tại, có thể dùng cấu trúc dữ liệu hoặc hash map.
    
    // Chỉ trả về -1 nếu không tìm thấy đối tác.
    return -1;  // Giả định không tìm thấy đối tác, cần điều chỉnh khi có logic tìm kiếm đối tác thực tế
}

// Hàm xử lý thông điệp video
void handle_video_message(int client_fd, const char* message) {
    char command[20];
    int peer_fd;
    
    // Phân tích thông điệp và tìm lệnh
    sscanf(message, "%s", command);
    
    if (strcmp(command, "OFFER") == 0) {
        char offer[256];
        sscanf(message + 6, "%s", offer);  // Bỏ qua "OFFER"
        
        // Tìm kiếm đối tác và gửi OFFER
        peer_fd = find_peer_for_client(client_fd);
        if (peer_fd != -1) {
            send_video_offer(client_fd, peer_fd, offer);
        } else {
            log_error("No peer found for client %d", client_fd);
        }
        
    } else if (strcmp(command, "ANSWER") == 0) {
        char answer[256];
        sscanf(message + 7, "%s", answer);  // Bỏ qua "ANSWER"
        
        // Tìm kiếm đối tác và gửi ANSWER
        peer_fd = find_peer_for_client(client_fd);
        if (peer_fd != -1) {
            send_video_answer(client_fd, peer_fd, answer);
        } else {
            log_error("No peer found for client %d", client_fd);
        }
        
    } else if (strcmp(command, "ICE") == 0) {
        char candidate[256];
        sscanf(message + 4, "%s", candidate);  // Bỏ qua "ICE"
        
        // Tìm kiếm đối tác và gửi ICE Candidate
        peer_fd = find_peer_for_client(client_fd);
        if (peer_fd != -1) {
            send_video_ice_candidate(client_fd, peer_fd, candidate);
        } else {
            log_error("No peer found for client %d", client_fd);
        }
        
    } else {
        log_error("Unknown video message: %s", message);
    }
}
