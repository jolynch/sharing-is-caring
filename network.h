#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

// Server methods
void bind_to_port(int socket, int port);
int open_listener_socket();

// Client methods
int open_socket(const char* ip, int port);
int send_message(const char* ip, int port, const uint8_t* msg, int len, uint8_t* rec);
int recv_data(int socket, uint8_t* rec, int len);
