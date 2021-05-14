#ifndef TCP_NONBLOCK_H
#define TCP_NONBLOCK_H

#ifdef _WIN32
#include <WinSock2.h>
#elif defined(UNIX) || defined(MACOS)
#include <sys/socket.h>
#endif


#if defined(UNIX) || defined(MACOS)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
#endif



int tcp_init_socket( void );
int tcp_set_nonblocking( SOCKET skt );
SOCKET tcp_get_max_socket( void );
SOCKADDR_IN tcp_setup_addr_server( int port );
int tcp_setup_listen_skt( SOCKET skt, int port );
SOCKADDR_IN tcp_setup_addr_client( const char* const ip, int port );
void tcp_close_socket( SOCKET skt );

void insert_socket( SOCKET skt );
void remove_socket( SOCKET skt );





void tcp_server_non_blocking( int port );
void tcp_client_blocking( const char* const ip, int port );
void tcp_client_non_blocking( const char* const ip, int port );


#endif
