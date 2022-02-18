#ifndef FFMPEG_CLIENT_H
#define FFMPEG_CLIENT_H

#include <string>
#include <srt.h>



//#define CLIENT_SEND
#define CLIENT_RECV



int ffmpeg_client( std::string ip, std::string port );


#ifdef CLIENT_SEND
void read_skip_stdin();
void client_send( SOCKET fhandle );
#elif defined(CLIENT_RECV)
void client_recv( SRTSOCKET handle );
#endif



#endif