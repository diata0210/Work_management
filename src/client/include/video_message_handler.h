#ifndef VIDEO_MESSAGE_HANDLER_H
#define VIDEO_MESSAGE_HANDLER_H

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

// Hàm gửi thông điệp video tới đối tác
void send_video_message_to_peer(int peer_fd, const char* message);

// Hàm gửi thông điệp VIDEO OFFER tới đối tác
void send_video_offer(int client_fd, int peer_fd, const char* offer);

// Hàm gửi thông điệp VIDEO ANSWER tới đối tác
void send_video_answer(int client_fd, int peer_fd, const char* answer);

// Hàm gửi thông điệp ICE Candidate tới đối tác
void send_video_ice_candidate(int client_fd, int peer_fd, const char* candidate);

// Hàm tìm kiếm đối tác cho client
int find_peer_for_client(int client_fd);

// Hàm xử lý thông điệp video
void handle_video_message(int client_fd, const char* message);

#endif // VIDEO_MESSAGE_HANDLER_H
