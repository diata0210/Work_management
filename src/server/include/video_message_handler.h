#ifndef VIDEO_MESSAGE_HANDLER_H
#define VIDEO_MESSAGE_HANDLER_H

#include <stdio.h>

void handle_offer(int client_fd, const char* offer);
void handle_answer(int client_fd, const char* answer);
void handle_ice_candidate(int client_fd, const char* candidate);
void handle_video_message(int client_fd, const char* message);
void send_to_peer(int peer_fd, const char* message);
int find_peer_for_client(int client_fd);

#endif // VIDEO_MESSAGE_HANDLER_H
