#ifndef SOCKET_H
#define SOCKET_H

int connect_to_server(const char *server_ip, int port);
int send_message(int client_fd, const char *message);
int receive_message(int client_fd, char *buffer, int buffer_size);

#endif  // SOCKET_H
