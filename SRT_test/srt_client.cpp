#include "srt_client.h"

#include <iostream>
#include <string>



using namespace std;



void test_client_func( SOCKET fhandle )
{
    char buf[1316];
    int res;
    int index;

    while(true)
    {        
        res = srt_recvmsg( fhandle, buf, 1316 );
        if( res <= 0 )
            break;
        memcpy( &index, buf, sizeof(index) );
        printf( "recv. res = %d. index = %d\n", res, index );

        res = srt_sendmsg2( fhandle, buf, 1316, nullptr );
        if( res <= 0 )
            break;
        printf("send. res = %d\n", res );
    }

    cout << "disconnect...\n";
}






void srt_client_test()
{
    std::string ip = "111.248.195.148", 
                port = "1234";

    //
    srt_startup();
    srt_setloglevel(srt_logging::LogLevel::debug);

    addrinfo hints, *peer;
    memset( &hints, 0, sizeof(addrinfo) );
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    SRTSOCKET fhandle = SRT_INVALID_SOCK; 

    if( 0 != getaddrinfo( ip.c_str(), port.c_str(), &hints, &peer ) )
    {
        cout << "error" << endl;
        return;
    }

    //
    fhandle = srt_create_socket();
    SRT_TRANSTYPE trans_type = SRTT_LIVE;
    srt_setsockopt( fhandle, 0, SRTO_TRANSTYPE, &trans_type, sizeof trans_type );
    
    int64_t maxbw = 0; 
    srt_setsockopt( fhandle, 0, SRTO_MAXBW, &maxbw, sizeof maxbw );
    
    int latency = 120;   
    srt_setsockopt( fhandle, 0, SRTO_LATENCY, &latency, sizeof latency );
    
    int recv_buf = 62768000;  // default = 8192
    srt_setsockopt( fhandle, 0, SRTO_RCVBUF, &recv_buf, sizeof recv_buf );
    
    int send_buf = 62768000;
    srt_setsockopt( fhandle, 0, SRTO_SNDBUF, &send_buf, sizeof send_buf );    
    
    while(true)
    {
        int flag = srt_connect( fhandle, peer->ai_addr, peer->ai_addrlen );
    
        if (SRT_ERROR == flag)
            cout << "wait..." << endl;
        else 
            break;
    }
    
    cout << "connect to " << ip << ":" << port << "\n";
    
    // start
    test_client_func(fhandle);
    
    //
    srt_close(fhandle);

    freeaddrinfo(peer);
    srt_cleanup();

    return;
}
