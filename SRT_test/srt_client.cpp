#include "srt_client.h"

#include <iostream>
#include <string>
#include <thread>


using namespace std;




/*
void test_recv2( SRTSOCKET handle )
{
    char recv_buf[1316] = {0};
    int recv_res;
    FILE *gp = fopen( "C:\\Tools\\stream.dat", "wb+");
    FILE *gp2 = fopen( "C:\\Tools\\time.txt", "w+");

    int index = 0;
    time_point<steady_clock,microseconds> timer;

    //while(true)
    for( ; index < 9000000; index++ )
    {
        printf("index = %d\n", index);

        recv_res = srt_recvmsg( handle, recv_buf, 1316 );
        timer = time_point_cast<microseconds>(steady_clock::now());
        if( recv_res > 0 )
        {
            fwrite( recv_buf, recv_res, 1, gp );
            fprintf( gp2, "%lld %d %d\n", timer.time_since_epoch().count(), index, recv_res );
        }
    }

    fclose(gp);
    fclose(gp2);
    
}*/


void test_function( SOCKET fhandle )
{
    char buf[1316];
    int res;

    while(true)
    {
        res = srt_recvmsg( fhandle, buf, 1316 );

        if( res <= 0 )
            break;        
        printf( "recv. res = %d\n", res );
        
        res = srt_sendmsg2( fhandle, buf, 1316, nullptr );
        if( res <= 0 )
            break;

        printf("send. res = %d\n", res );
    }

    cout << "disconnect...\n";
}






void srt_client_test()
{
    std::string ip = "127.0.0.1", 
                port = "1234";

    //
    srt_startup();
    srt_setloglevel(srt_logging::LogLevel::debug);

    addrinfo hints, *peer;
    memset( &hints, 0, sizeof(addrinfo) );
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    SRTSOCKET fhandle = -1; 

    if( 0 != getaddrinfo( ip.c_str(), port.c_str(), &hints, &peer ) )
    {
        cout << "error" << endl;
        return;
    }

    while(true)
    {
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
        test_function(fhandle);

        //
        srt_close(fhandle);
        break;
    }


    freeaddrinfo(peer);
    srt_cleanup();

    return;
}
