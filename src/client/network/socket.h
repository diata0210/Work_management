#ifndef SOCKET_H
#define SOCKET_H
#include <stdbool.h>

// int connect_to_server(const char *server_ip, int port);
// int send_message(int client_fd, const char *message);
// int receive_message(int client_fd, char *buffer, int buffer_size);
bool initialize_socket();
void close_socket();
bool send_request(const char *request, char *response);
bool receive_response(char *response, int max_length);
#endif  // SOCKET_H
