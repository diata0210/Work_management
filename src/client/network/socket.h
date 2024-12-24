#ifndef SOCKET_H
#define SOCKET_H

#include <stdbool.h>


void initialize_socket_mutex();
void destroy_socket_mutex();
bool initialize_socket(const char *server_ip, int port);
void close_socket();
bool send_request(const char *request, char *response);
bool receive_response(char *response, int max_length);
bool receive_data_async(void (*callback)(const char *data)) ;
void *start_receive_loop(void *callback) ;
bool send_request_no_response(const char *request) ;

#endif // SOCKET_H
