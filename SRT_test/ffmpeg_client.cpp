
#include "ffmpeg_client.h"

#include <iostream>

#include <cassert>
#include <string>

#include <algorithm>
#include <thread>


#include <io.h>
#include <fcntl.h>


using namespace std;



#ifdef CLIENT_SEND
bool g_cnn_start = false;
bool g_hold_flag = false;
#endif



#ifdef CLIENT_RECV
void client_recv( SRTSOCKET handle )
{
    char buf[1316] = {0};
    int res;

    while(true)
    {
        res = srt_recvmsg2( handle, buf, 1316, nullptr );
        if( res <= 0 )
            break;
        fwrite( buf, res, 1, stdout );
        //printf("res = %d, buf[0] = %d\n", res, buf[0]);               
    }

    printf("end client recv.\n");
}
#endif








#ifdef CLIENT_SEND
void client_send( SOCKET fhandle )
{
    char buf[1316];
    int res;

    while(g_hold_flag == false);

    while(true)
    {
        int res = fread( buf, 1316, 1, stdin );
        if( res <= 0 )
            break;

        res = srt_sendmsg2( fhandle, buf, 1316, nullptr );
        if( res <= 0 )
            break;
    }
}
#endif




#ifdef CLIENT_SEND
// 跳過這些資料 (要等待連線)
void read_skip_stdin()
{
    char buf[1316];    
    while( g_cnn_start == false )
        fread( buf, 1316, 1, stdin );  
    g_hold_flag = true;
}
#endif






int ffmpeg_client( std::string ip, std::string port )
{
#ifdef CLIENT_SEND
    _setmode( _fileno(stdin), O_BINARY );
#elif defined(CLIENT_RECV)
    _setmode( _fileno(stdout), O_BINARY );
#else
#error error.
#endif
    
    //
    srt_startup();
    srt_setloglevel(srt_logging::LogLevel::debug);

    struct addrinfo hints, * peer;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    SRTSOCKET fhandle = -1; // = srt_create_socket();

    if( 0 != getaddrinfo( ip.c_str(), port.c_str(), &hints, &peer ) )
    {
        cout << "error" << endl;
        return -1;
    }

    while(true)
    {
        fhandle = srt_create_socket();
        SRT_TRANSTYPE live_mode = SRTT_LIVE;
        srt_setsockopt( fhandle, 0, SRTO_TRANSTYPE, &live_mode, sizeof live_mode );

        int64_t maxbw = -1; 
        srt_setsockopt( fhandle, 0, SRTO_MAXBW, &maxbw, sizeof maxbw );

        int recv_buf = 62768000;  // default = 8192
        srt_setsockopt(fhandle, 0, SRTO_RCVBUF, &recv_buf, sizeof recv_buf );

        int send_buf = 62768000;  // default = 8192
        srt_setsockopt(fhandle, 0, SRTO_SNDBUF, &send_buf, sizeof send_buf );

        while(true)
        {
            int flag = srt_connect(fhandle, peer->ai_addr, peer->ai_addrlen);

            if( SRT_ERROR != flag ) 
                break;
        }

        // start
#ifdef CLIENT_SEND
        g_cnn_start = true;
        client_send(fhandle);
#elif defined(CLIENT_RECV)
        client_recv(fhandle);
#else
#error error.
#endif
        srt_close(fhandle);
        break;
    }

    freeaddrinfo(peer);
    srt_cleanup();

    return 0;
}
