#ifndef TCP_NONBLOCK_H
#define TCP_NONBLOCK_H

#include <WinSock2.h>

int tcp_init_socket( void );
int tcp_set_nonblocking( SOCKET skt );
SOCKET tcp_get_max_socket( fd_set set );

void tcp_server_non_blocking( int port );
void tcp_client_non_blocking( const char* const ip, int port );

#endif
