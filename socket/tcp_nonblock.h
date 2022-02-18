#ifndef TCP_NONBLOCK_H
#define TCP_NONBLOCK_H

#ifdef _WIN32
#include <WinSock2.h>
#elif defined(UNIX) || defined(MACOS)
#include <sys/socket.h>
#include <netinet/in.h>  // use for macos. otherwise, sockaddr_in will cause compile error
#endif


#if defined(UNIX) || defined(MACOS)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
#endif



int tcp_init_socket( void );
int tcp_set_nonblocking( SOCKET skt );
SOCKET tcp_get_max_socket( void );
int tcp_setup_listen_skt( SOCKET skt, int port );
void tcp_close_socket( SOCKET skt );

SOCKADDR_IN tcp_setup_addr_server( int port );
SOCKADDR_IN tcp_setup_addr_client( const char* const ip, int port );



void insert_socket( SOCKET skt );
void remove_socket( SOCKET skt );



void tcp_show_ip_address( SOCKET skt );




void tcp_server_non_blocking( int port );
SOCKET tcp_server_accept( SOCKET listen_skt );
void tcp_server_rs( SOCKET skt );

void tcp_client_blocking( const char* const ip, int port );

void tcp_client_non_blocking( const char* const ip, int port );
void tcp_client_connect_test_1( SOCKET skt, SOCKADDR_IN addr );
void tcp_client_connect_test_2( SOCKET skt, SOCKADDR_IN addr );
void tcp_client_connect_test_3( SOCKET skt, SOCKADDR_IN addr );
void tcp_client_connect_test_4( SOCKET skt, SOCKADDR_IN addr );


#endif
