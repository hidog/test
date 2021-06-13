#include "srt_server.h"

#include <iostream>
#include <string>


using namespace std;



static void srt_log_func(void* opaque, int level, const char* file, int line, const char* area, const char* message)
{
    printf( "level = %d, file = %s, line = %d, area = %s, message = %s", level, file, line, area, message );

    char str[1000];
    sprintf_s( str, 1000, "message = %s\n", message );
    OutputDebugStringA(str);
}







SRTSOCKET init_srt( std::string port )
{
    // note : call srt_startup once. In multi-thread, need add flag to control, don't call more than once.
    srt_startup();

    srt_setloglevel(srt_logging::LogLevel::debug);
    //srt_setloghandler( nullptr, srt_log_func );
    
    addrinfo hints;
    addrinfo* res;

    memset( &hints, 0, sizeof(addrinfo) );
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    if( 0 != getaddrinfo( NULL, port.c_str(), &hints, &res) )
    {
        std::cout << "error getaddrinfo" << std::endl;
        return SRT_INVALID_SOCK;
    }

    SRTSOCKET serv = srt_create_socket();

    SRT_TRANSTYPE live_mode = SRTT_LIVE;
    srt_setsockopt( serv, 0, SRTO_TRANSTYPE, &live_mode, sizeof(live_mode));

    int latency = 120;
    srt_setsockopt(serv, 0, SRTO_LATENCY, &latency, sizeof(latency));

    //
    if( SRT_ERROR == srt_bind(serv, res->ai_addr, res->ai_addrlen) )
    {
        std::cout << "error srt_bind" << std::endl;
        return SRT_INVALID_SOCK;
    }

    freeaddrinfo(res);

    cout << "server is ready at port: " << port << endl;
    return serv;
}






SRTSOCKET accept_srt( SRTSOCKET serv )
{
    srt_listen( serv, 1000 );

    sockaddr_storage client_addr;
    int addrlen = sizeof(client_addr);

    SRTSOCKET fhandle;

    while(true)
    {
        fhandle = srt_accept( serv, (sockaddr*)&client_addr, &addrlen );
        if( SRT_INVALID_SOCK == fhandle )
        {
            std::cout << "accept error : " << srt_getlasterror_str() << std::endl;
            return SRT_INVALID_SOCK;
        }

        char clienthost[NI_MAXHOST];
        char clientservice[NI_MAXSERV];
        getnameinfo( (sockaddr*)&client_addr, addrlen, clienthost, sizeof(clienthost), clientservice, sizeof(clientservice), NI_NUMERICHOST | NI_NUMERICSERV );
        std::cout << "new connection : " << clienthost << " : " << clientservice << std::endl;

        break;
    }

    int send_buf = 627680;  // default = 8192
    srt_setsockopt( fhandle, 0, SRTO_SNDBUF, &send_buf, sizeof send_buf );

    int recv_buf = 627680;  // default = 8192
    srt_setsockopt( fhandle, 0, SRTO_RCVBUF, &recv_buf, sizeof recv_buf );

    int64_t maxbw = 0; 
    srt_setsockopt( fhandle, 0, SRTO_MAXBW, &maxbw, sizeof maxbw );

    return fhandle;
}







void test_server_func( SRTSOCKET handle )
{
    char buf[1316] = {0};
    int res;

    while(true)
    {      
        res = srt_sendmsg2( handle, buf, 1316, nullptr );
        if( res <= 0 )
            break;
        printf( "send res = %d\n", res );

        res = srt_recvmsg( handle, buf, 1316 );
        if( res <= 0 )
            break;
        printf( "recv res = %d\n", res ); 
    }

    cout << "disconnected....\n";
}






void srt_server_test()
{
    SRTSOCKET handle;

    std::string port = "1234";
    SRTSOCKET serv =init_srt(port);

    //
    handle = accept_srt(serv);

    // 移除的話會出錯, 看起來需要等一下才能開始送資料
    // 理論上能避免這個問題才對
    // 再研究一下相關的code
    Sleep(10);

    //
    test_server_func( handle );

    // note: call srt_close, it will not send data in buffer. 
    // 理論上能讓他送完資料才關閉. 有空再研究.
    srt_close(handle);  

    srt_close(serv);
    srt_cleanup();

    return;
}
