#include "ffmpeg_server.h"

#include <iostream>

#include <io.h>
#include <fcntl.h>

#include <thread>



#ifdef SERVER_SEND
bool g_serv_start = false;
bool g_wait_flag = false;
#endif








SRTSOCKET ffmpeg_init_srt( std::string port )
{
    // note : call srt_startup once. In multi-thread, need add flag to control, don't call more than once.
    srt_startup();
    srt_setloglevel(srt_logging::LogLevel::debug);

    addrinfo hints;
    addrinfo* res;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    if( 0 != getaddrinfo(NULL, port.c_str(), &hints, &res) )
        return -1;    

    SRTSOCKET serv = srt_create_socket();

    SRT_TRANSTYPE live_mode = SRTT_LIVE;
    srt_setsockopt(serv, 0, SRTO_TRANSTYPE, &live_mode, sizeof(live_mode));

    if( SRT_ERROR == srt_bind(serv, res->ai_addr, res->ai_addrlen) )
    {
        std::cout << "error srt_bind" << std::endl;
        return -1;
    }

    freeaddrinfo(res);
    return serv;
}





SRTSOCKET ffmpeg_accept_srt( SRTSOCKET serv )
{
    srt_listen(serv, 1000);

    sockaddr_storage clientaddr;
    int addrlen = sizeof(clientaddr);

    SRTSOCKET fhandle;

    while (true)
    {
        fhandle = srt_accept(serv, (sockaddr*)&clientaddr, &addrlen);
        if (SRT_INVALID_SOCK == fhandle)
            return -1;        
        break;
    }

    int send_buf = 62768000;  // default = 8192
    srt_setsockopt(fhandle, 0, SRTO_SNDBUF, &send_buf, sizeof send_buf );

    int recv_buf = 62768000;  // default = 8192
    srt_setsockopt(fhandle, 0, SRTO_RCVBUF, &recv_buf, sizeof recv_buf );

    int64_t maxbw = 0; 
    srt_setsockopt(fhandle, 0, SRTO_MAXBW, &maxbw, sizeof maxbw );

    return fhandle;
}





#ifdef SERVER_SEND
// 跳過這些資料 (要等待caller連線進來)
void read_from_stdin()
{
    char buf[1316];    
    while( g_serv_start == false )
        fread( buf, 1316, 1, stdin );  
    g_wait_flag = true;
}
#endif



#ifdef SERVER_SEND
void server_send_data( SOCKET fhandle )
{
    char buf[1316];
    int res;

    while( g_wait_flag == false );

    while(true)
    {
        int res = fread( buf, 1316, 1, stdin );
        if( res <= 0 )
            break;

        res = srt_sendmsg2( fhandle, buf, 1316, nullptr );
        if( res <= 0 )
            break;
    }

    printf( "end server send.\n" );
}
#endif









#ifdef SERVER_RECV
void server_recv( SRTSOCKET handle )
{
    char buf[1316]; 

    while(true)
    {
        int res = srt_recvmsg( handle, buf, 1316 );
        fwrite( buf, res, 1, stdout );

        if( res <= 0 )
            break;
    }
}
#endif









int ffmpeg_server( std::string port )
{
#ifdef SERVER_RECV
    _setmode( _fileno(stdout), O_BINARY );
#elif defined(SERVER_SEND)
    _setmode( _fileno(stdin), O_BINARY );
#else
#error error.
#endif

#ifdef SERVER_SEND
    std::thread thr(read_from_stdin);
#endif

    SRTSOCKET handle;
    SRTSOCKET serv = ffmpeg_init_srt(port);

    handle = ffmpeg_accept_srt(serv);
    if( handle == -1 )
        return -1;

#ifdef SERVER_RECV
    server_recv(handle);
#elif defined(SERVER_SEND)
    g_serv_start = true;
    server_send_data(handle);
#else
#error error.
#endif

    srt_close(handle);  // note: call srt_close, it will not send data in buffer.
    srt_close(serv);
    srt_cleanup();

    return 0;
}
