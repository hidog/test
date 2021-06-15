#ifndef FFMPEG_SERVER_H
#define FFMPEG_SERVER_H


#include <srt.h>
#include <cstring>


#define SERVER_SEND
//#define SERVER_RECV


int ffmpeg_server( std::string port );

SRTSOCKET ffmpeg_init_srt( std::string port );
SRTSOCKET ffmpeg_accept_srt( SRTSOCKET serv );

#ifdef SERVER_RECV
void server_recv( SRTSOCKET handle );
#elif defined(SERVER_SEND)
void read_from_stdin();
void server_send_data( SOCKET fhandle );
#endif




#endif