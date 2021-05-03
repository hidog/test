#include "udp_example.h"
#include <stdio.h>
#include <string>
#include <string.h>
#include <chrono>
#include <thread>

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#elif defined(UNIX) || defined(MACOS)
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#endif


/*
https://man7.org/linux/man-pages/man2/bind.2.html
https://www.mdeditor.tw/pl/p8ys/zh-tw
https://codertw.com/%E7%A8%8B%E5%BC%8F%E8%AA%9E%E8%A8%80/624590/
https://codertw.com/%E5%89%8D%E7%AB%AF%E9%96%8B%E7%99%BC/392331/
*/


#if defined(UNIX) || defined(MACOS)
typedef int SOCKET;
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#elif defined(_WIN32)
#define bzero(ptr,size)     memset( (ptr), 0, (size) )
typedef int socklen_t;
typedef int ssize_t; // 查了一下windows沒找到ssize_t這個定義,在研究一下. 看討論目前非標準定義
#endif



struct OrderData
{
    int order_index;
    char filling[1490];
};


using namespace std::chrono;


struct RTT_Data
{
    int index;
    int64_t time_stamp;
};


struct NonBkData
{
    int index;
    int port;
    char message[1400];
};





void udp_nonblockint_client()
{
    srand( (unsigned int)time(NULL) );
    
#ifdef _WIN32
    WORD socket_version = MAKEWORD(2,2);
    WSADATA wsa_data; 
    if( WSAStartup( socket_version, &wsa_data ) != 0 )
    {
        printf("init error\n");
        return;
    }
#endif

    int res;
#ifdef _WIN32
    /* 
        If blcok_mode = 0, blocking is enabled; 
        If blcok_mode != 0, non-blocking mode is enabled.
    */
    u_long block_mode = 1;
#else
    int skt_flags;
#endif

    // create socket
    SOCKET client_skt[3], max_skt = -1;
    for( int i = 0; i < 3; i++ )
    {
        client_skt[i] = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP );
        if( client_skt[i] == INVALID_SOCKET )
        {
            printf("create socket error.\n");
            return;
        }
        if( client_skt[i] > max_skt )
            max_skt = client_skt[i];

#ifdef _WIN32
        res = ioctlsocket( client_skt[i], FIONBIO, &block_mode );
        if( res != NO_ERROR )
        {
            printf("set non-block fail\n");
            return;
        }
#else
        skt_flags = fcntl( client_skt[i], F_GETFL, 0 );
        skt_flags |= O_NONBLOCK;
        res = fcntl( client_skt[i], F_SETFL, skt_flags );
        if( res == -1 )
        {
            printf("set non-block fai.\n");
            return;
        }
#endif
    }
    

    // set remote address, port
    sockaddr_in remote_addr;
    bzero( &remote_addr, sizeof remote_addr );
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_addr.s_addr = inet_addr( "127.0.0.1" );
    socklen_t remote_len = sizeof(remote_addr);

    //
    NonBkData nbk_data;
    const int nbk_data_size = sizeof( NonBkData );
    char *send_buf = new char[nbk_data_size];
    if( send_buf == NULL )
    {
        printf("malloc fail.\n");
        return;
    }

    ssize_t ret;
    //char recv_buf[500];

    //
    fd_set write_set;
    int select_ret;
    timeval timeout = { 2, 500000 };
    int total_count = 0;
    
    //
    while(1)
    {
        FD_ZERO( &write_set );
        for( int i = 0; i < 3; i++ )
        {
            FD_SET( client_skt[i], &write_set );
        }
        
        select_ret = select( max_skt+1, NULL, &write_set, NULL, &timeout );
        if( select_ret == SOCKET_ERROR )
        {
            printf("select error\n");
            break;
        }
        
        for( int i = 0; i < 3; i++ )
        {
            if( FD_ISSET( client_skt[i], &write_set ) )
            {
                int port = 27271 + rand() % 10;
                remote_addr.sin_port = htons(port);
                
                nbk_data.index = i;
                nbk_data.port = port;
                sprintf( nbk_data.message, 
                         "This is client. port = %d, index = %d, total_count = %d", 
                         nbk_data.port, nbk_data.index, total_count++ );
                memcpy( send_buf, &nbk_data, nbk_data_size );
                ret = sendto( client_skt[i], send_buf, nbk_data_size, 0, (sockaddr*)&remote_addr, remote_len );
                printf("send %d, i = %d, port = %d\n", (int)ret, i, port );
                
                /*ret = recvfrom( client_skt[i], recv_buf, 500, 0, (sockaddr*)&remote_addr, &remote_len );
                printf("recv %d, msg = %s\n", (int)ret, recv_buf );*/
            }
        }
        printf("\n");
        
#ifdef _WIN32
        Sleep(1000);
#else
        sleep(1);
#endif
    }

    delete [] send_buf;
    send_buf = NULL;

#ifdef _WIN32
    for( int i = 0; i < 3; i++ )
        closesocket( client_skt[i] );
    WSACleanup();
#elif defined(UNIX) || defined(MACOS)
    for( int i = 0; i < 3; i++ )
        close(client_skt[i]);
#endif
    
}





void udp_nonblocking_server()
{
#ifdef _WIN32
    WORD socket_version = MAKEWORD(2,2);
    WSADATA wsa_data; 
    if( WSAStartup(socket_version,&wsa_data) != 0 )
    {
        printf("init error\n");
        return;
    }
#endif

    // 
    sockaddr_in local_addr;
    SOCKET server_skt[10];
    SOCKET max_skt = -1;
    int res;

#ifdef _WIN32
    /* 
        If blcok_mode = 0, blocking is enabled; 
        If blcok_mode != 0, non-blocking mode is enabled.
    */
    u_long blcok_mode = 1;
#else
    int skt_flags;
#endif

    for( int i = 0; i < 10; i++ )
    {
        // create socket
        server_skt[i] = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP );
        if( max_skt < server_skt[i] )
            max_skt = server_skt[i];
        
        // set non-blocking
#ifdef _WIN32
        res = ioctlsocket( server_skt[i], FIONBIO, &blcok_mode );
        if( res != NO_ERROR )
        {
            printf("set non-block fail\n");
            return;
        }
#else
        skt_flags = fcntl( server_skt[i], F_GETFL, 0 );
        skt_flags |= O_NONBLOCK;
        res = fcntl( server_skt[i], F_SETFL, skt_flags );
        if( res == -1 )
        {
            printf("set non-block fail.\n");
            return;
        }
        //fcntl( server_skt[i], F_SETFL, O_NONBLOCK ); 也可以用這段code直接設置 non-blocking
#endif
        
        //
        bzero( &local_addr, sizeof local_addr );  // windows沒有bzero
        local_addr.sin_family = AF_INET;
        local_addr.sin_port = htons( (27271+i) );
        local_addr.sin_addr.s_addr = INADDR_ANY; 
        int local_len = sizeof(local_addr);

        // 
        if( bind( server_skt[i], (sockaddr*)&local_addr, local_len ) == SOCKET_ERROR )
        {
            printf("bind error %d!\n", i );
            return;
        }
    }
    
    printf( "bind success. wait for client data...\n" );

    // receive remote data
    sockaddr_in remote_addr;
    socklen_t remote_len = sizeof(remote_addr);

    const int nbk_data_size = sizeof(NonBkData);
    char *recv_buf = new char[nbk_data_size];
    if( recv_buf == NULL )
    {
        printf("malloc error\n");
        return;
    }
    ssize_t ret;
    NonBkData nbk_data;
    
    //
    fd_set read_set;
    timeval timeout = { 2, 500000 };
    int select_ret;
    //char send_buf[300];
    //int total_count = 0;
    
    // start
    while(true)
    {
        FD_ZERO( &read_set );
        for( int i = 0; i < 10; i++ )
        {
            FD_SET( server_skt[i], &read_set );
        }
        
        select_ret = select( max_skt+1, &read_set, NULL, NULL, &timeout );
        if( select_ret == SOCKET_ERROR )
        {
            printf("select error\n");
            break;
        }
        
        for( int i = 0; i < 10; i++ )
        {
            if( FD_ISSET( server_skt[i], &read_set) )
            {
                bzero( &remote_addr, sizeof remote_addr );
                ret = recvfrom( server_skt[i], recv_buf, nbk_data_size, 0, (sockaddr*)&remote_addr, &remote_len );
                if( ret <= 0 )
                    printf("error, ret = %d\n", (int)ret );
                else
                {
                    memcpy( &nbk_data, recv_buf, nbk_data_size );
                    printf("ret = %d, rec from %s %d\n", (int)ret, inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port) );
                    printf("index = %d, port = %d, msg = %s\n", nbk_data.index, nbk_data.port, nbk_data.message );
                    
                    /*sprintf( send_buf, "Hello this is server. count = %d", total_count++ );
                    ret = sendto( server_skt[i], send_buf, 300, 0, (sockaddr*)&remote_addr, remote_len );
                    printf("send back. ret = %d\n", (int)ret );*/
                }
            }
        }
        
        printf("\n");
    }

    delete [] recv_buf;
    recv_buf = NULL;

#ifdef _WIN32
    for( int i = 0; i < 10; i++ )
        closesocket(server_skt[i]);
    WSACleanup();
#elif defined(UNIX) || defined(MACOS)
    for( int i = 0; i < 10; i++ )
        close(server_skt[i]);
#endif
}







void udp_RTT_client()
{
#ifdef _WIN32
    // windows need init
    WORD socket_version = MAKEWORD(2,2);
    WSADATA wsa_data; 
    if( WSAStartup( socket_version, &wsa_data ) != 0 )
    {
        printf("init error\n");
        return;
    }
#endif

    // create socket
    SOCKET client_skt = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP );
    if( client_skt == INVALID_SOCKET )
    {
        printf("create socket error.\n");
        return;
    }
   
    // set remote address, port
    sockaddr_in remote_addr;
    bzero( &remote_addr, sizeof remote_addr );
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(6178);
#ifdef _WIN32
    inet_pton( AF_INET, "122.116.84.59", &remote_addr.sin_addr );  // 另一個作法
#else
    inet_pton( AF_INET, "36.226.248.130", &remote_addr.sin_addr );  // 另一個作法
#endif
    socklen_t remote_len = sizeof(remote_addr);

    // send data
    char *send_buf = new char[sizeof(RTT_Data)];
    char *recv_buf = new char[sizeof(RTT_Data)];
    ssize_t ret;
    RTT_Data rtt_data, rtt_recv;
    int index = 0;
    bool first_flag = true;
    int64_t max = 0;
    
    while(true)
    {
        rtt_data.index = index;
        rtt_data.time_stamp = time_point_cast<milliseconds>(steady_clock::now()).time_since_epoch().count();
        memcpy( send_buf, &rtt_data, sizeof(RTT_Data) );
        
        ret = sendto( client_skt, send_buf, sizeof(RTT_Data), 0, (sockaddr*)&remote_addr, remote_len );
        if( ret <= 0 )
        {
            printf( "send end. %d\n", (int)ret );
            break;
        }
        
        //
        if( first_flag == true )
        {
            first_flag = false;
#ifdef _WIN32
            int timeout = 20500;
#else
            timeval timeout = { 20, 500000 };
#endif
            socklen_t timeout_len = sizeof(timeval);
            int set_res = setsockopt( client_skt, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, timeout_len );
            if( set_res == SOCKET_ERROR )
            {
                printf( "set timeout fail %d\n", set_res );
                break;
            }
        }
        
        //
        ret = recvfrom( client_skt, recv_buf, sizeof(RTT_Data), 0, (sockaddr*)&remote_addr, &remote_len );
        if( ret < 0 )
        {
#ifdef _WIN32
            int err = WSAGetLastError();
#else
            int err = errno;
#endif
            printf("recv timeout. %d, err code = %d\n", (int)ret, err );
            continue;
        }
        else if( ret == 0 )
        {
            printf("recv end.\n");
            break;
        }
        
        memcpy( &rtt_recv, recv_buf, sizeof(RTT_Data) );
        if( rtt_recv.index != index )
        {
            printf("index diff. %d != %d. error\n", rtt_recv.index, index );
            break;
        }
        
        auto time_now = time_point_cast<milliseconds>(steady_clock::now()).time_since_epoch().count();
        auto diff = time_now - rtt_recv.time_stamp;
        if( diff > max )
            max = diff;
        printf("index = %d, RTT = %lld, max = %lld\n", index, (long long int)diff, (long long int)max );
        
        //std::this_thread::sleep_for( milliseconds(50) );

        index++;
    }
    
    delete [] send_buf;
    send_buf = nullptr;
    
    delete [] recv_buf;
    recv_buf = nullptr;

#ifdef _WIN32
    closesocket( client_skt );
    WSACleanup();
#elif defined(UNIX) || defined(MACOS)
    close(client_skt);
#endif
    
}



void udp_RTT_server()
{
#ifdef _WIN32
    // windows need init
    WORD socket_version = MAKEWORD(2,2);
    WSADATA wsa_data;
    if( WSAStartup(socket_version,&wsa_data) != 0 )
    {
        printf("init error\n");
        return;
    }
#endif

    // bind server socket
    SOCKET server_skt = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP );
    
    sockaddr_in local_addr;
    bzero( &local_addr, sizeof local_addr );  // windows沒有bzero
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(6178);

#ifdef _WIN32
    local_addr.sin_addr.S_un.S_addr = INADDR_ANY;
#else
    local_addr.sin_addr.s_addr = INADDR_ANY;
#endif
    int local_len = sizeof(local_addr);
    
    //
    if( bind(server_skt, (sockaddr*)&local_addr, local_len ) == SOCKET_ERROR )
    {
        printf("bind error !\n");
        return;
    }

    printf( "bind success. wait for client data...\n" );

    // receive remote data
    sockaddr_in remote_addr;
    bzero( &remote_addr, sizeof remote_addr );
    socklen_t remote_len = sizeof(remote_addr);

    char *recv_buf = new char[sizeof(RTT_Data)];
    char *send_buf = new char[sizeof(RTT_Data)];
    ssize_t ret;
    RTT_Data rtt_data;
    bool first_flag = true;
    
    while(true)
    {
        ret = recvfrom( server_skt, recv_buf, sizeof(RTT_Data), 0, (sockaddr*)&remote_addr, &remote_len );
        if( ret < 0 )
        {
#ifdef _WIN32
            int err = WSAGetLastError();
#else
            int err = errno;
#endif
            printf("recv timeout %d, err = %d\n", (int)ret, err );
            continue;
        }
        else if( ret == 0 )
        {
            printf("recv end. %d\n", (int)ret );
            break;
        }
        
        if( first_flag == true )
        {
            first_flag = false;
#ifdef _WIN32
            int timeout = 20500;
#else
            timeval timeout = { 20, 500000 }; // 20.500 s
#endif
            socklen_t timeout_len = sizeof(timeval);
            auto set_res = setsockopt( server_skt, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, timeout_len );
            if( set_res == SOCKET_ERROR )
            {
                printf("set timeout fail. set_res = %d\n", set_res );
                break;
            }
        }
        
        memcpy( &rtt_data, recv_buf, sizeof(RTT_Data) );
        printf("recv data %d. index = %d, hour since epoch = %lld\n", (int)ret, rtt_data.index, (long long int)rtt_data.time_stamp );
        
        // prepare send back data;
        memcpy( send_buf, &rtt_data, sizeof(RTT_Data) );
        ret = sendto( server_skt, send_buf, sizeof(RTT_Data), 0, (sockaddr*)&remote_addr, remote_len );
        if( ret < 0 )
        {
            printf("send end. %d\n", (int)ret);
            break;
        }
    }
    
    delete [] recv_buf;
    recv_buf = nullptr;
    
    delete [] send_buf;
    send_buf = nullptr;

#ifdef _WIN32
    closesocket(server_skt);
    WSACleanup();
#elif defined(UNIX) || defined(MACOS)
    close(server_skt);
#endif
}







void udp_bind_client_socket( int skt )
{
    // 用底下的bind可以做到指定client port的效果,但不設定也會自動設定port.
    sockaddr_in local_addr;
    bzero(&local_addr, sizeof local_addr);  // windows沒有bzero
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(24256);  // local port指定24256

#ifdef _WIN32
    local_addr.sin_addr.S_un.S_addr = INADDR_ANY; // inet_addr("111.248.195.94");
#else
    local_addr.sin_addr.s_addr = INADDR_ANY;
#endif
    int local_len = sizeof(local_addr);

    // https://man7.org/linux/man-pages/man2/bind.2.html   see how to handle error
    if( bind(skt, (sockaddr*)&local_addr, local_len) == SOCKET_ERROR )
    { 
        printf("bind error !\n");
        return;
    }
}





void udp_hello_client( std::string ip, int port )
{
#ifdef _WIN32
    // windows need init
    WORD socket_version = MAKEWORD(2,2);
    WSADATA wsa_data; 
    if( WSAStartup( socket_version, &wsa_data ) != 0 )
    {
		printf("init error\n");
        return;
    }
#endif

    // create socket
    SOCKET client_skt = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP );
    if( client_skt == INVALID_SOCKET )
    {
        printf("create socket error.\n");
        return;
    }
   
    // 用來指定本地端的port
    //udp_bind_client_socket(client_skt);

    // set remote address, port
    sockaddr_in remote_addr;
    bzero( &remote_addr, sizeof remote_addr );
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(port);
#ifdef _WIN32
    remote_addr.sin_addr.S_un.S_addr = inet_addr( ip.c_str() );  
#elif defined(UNIX) || defined(MACOS)
    remote_addr.sin_addr.s_addr = inet_addr( ip.c_str() );
    //inet_pton( AF_INET, "127.0.0.1", &servaddr.sin_addr );  // 另一個作法
#endif
    socklen_t remote_len = sizeof(remote_addr);

    // send data
    char send_data[100] = "hello, server. this is client\n";
    ssize_t ret;
    ret = sendto( client_skt, send_data, strlen(send_data), 0, (sockaddr *)&remote_addr, remote_len );
    printf( "client send, ret = %ld\n", ret );

    // receive back
    char recv_data[100] = {0};
    ret = recvfrom( client_skt, recv_data, 100, 0, (sockaddr *)&remote_addr, &remote_len );
    if( ret > 0 )
    {
        printf( "recv from ip = %s, port = %d\n", inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port) );
        printf( "receive message from server. ret = %ld, data = %s\n", ret, recv_data );
    }
    else    
        printf( "ret = %ld <= 0. error\n", ret );

#ifdef _WIN32
    closesocket( client_skt );
    WSACleanup();
#elif defined(UNIX) || defined(MACOS)
    close(client_skt);
#endif
}







void udp_hello_client_c( std::string ip, int port )
{
#ifdef _WIN32
    // windows need init
    WORD socket_version = MAKEWORD(2,2);
    WSADATA wsa_data;
    if( WSAStartup( socket_version, &wsa_data ) != 0 )
    {
        printf("init error\n");
        return;
    }
#endif

    // create socket
    SOCKET client_skt = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP );
    if( client_skt == INVALID_SOCKET )
    {
        printf("create socket error.\n");
        return;
    }

    // set remote address, port
    sockaddr_in remote_addr;
    bzero( &remote_addr, sizeof remote_addr );
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(port);
#ifdef _WIN32
    remote_addr.sin_addr.S_un.S_addr = inet_addr( ip.c_str() );
#elif defined(UNIX) || defined(MACOS)
    remote_addr.sin_addr.s_addr = inet_addr( ip.c_str() );
    //inet_pton( AF_INET, "127.0.0.1", &servaddr.sin_addr );  // 另一個作法
#endif
    socklen_t remote_len = sizeof(remote_addr);
    
    /*
        udp可以使用connect,之後用read/write來做處理
     */
    if( connect( client_skt, (sockaddr *)&remote_addr, remote_len ) == -1 )
    {
        printf("connect fail.\n");
        return;
    }

    // send data
    char send_data[100] = "hello, server. this is client\n";
    ssize_t ret;
#if defined(UNIX) || defined(MACOS)
    ret = write( client_skt, send_data, strlen(send_data) );
#elif defined(_WIN32)
    ret = send( client_skt, send_data, strlen(send_data), 0 );
#endif
    printf( "client send, ret = %ld\n", ret );

    // receive back
    char recv_data[100] = {0};
#if defined(UNIX) || defined(MACOS)
    ret = read( client_skt, recv_data, 100 );
#elif defined(_WIN32)
    ret = recv( client_skt, recv_data, 100, 0 );
#endif
    if( ret > 0 )
    {
        printf( "recv from ip = %s, port = %d\n", inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port) );
        printf( "receive message from server. ret = %ld, data = %s\n", ret, recv_data );
    }
    else
        printf( "ret = %ld <= 0. error\n", ret );

#ifdef _WIN32
    closesocket( client_skt );
    WSACleanup();
#else
    close(client_skt);
#endif
    
}




void udp_hello_server( int port )
{
#ifdef _WIN32
    // windows need init
	WORD socket_version = MAKEWORD(2,2);
    WSADATA wsa_data; 
    if( WSAStartup(socket_version,&wsa_data) != 0 )
    {
		printf("init error\n");
        return;
    }
#endif

    // bind server socket
    SOCKET server_skt = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP );
    
    sockaddr_in local_addr;
    bzero( &local_addr, sizeof local_addr );  // windows沒有bzero
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(port);

#ifdef _WIN32
    local_addr.sin_addr.S_un.S_addr = INADDR_ANY; // inet_addr("111.248.195.94");
#elif defined(UNIX) || defined(MACOS)
    local_addr.sin_addr.s_addr = INADDR_ANY; 
#endif
    int local_len = sizeof(local_addr);
    
    // https://man7.org/linux/man-pages/man2/bind.2.html   see how to handle error
    if( bind(server_skt, (sockaddr*)&local_addr, local_len ) == SOCKET_ERROR )
    {
        printf("bind error !\n");
#ifdef _WIN32
        closesocket(server_skt);
        WSACleanup();
#elif defined(UNIX) || defined(MACOS)
        close(server_skt);
#endif
        return;
    }

    printf( "bind success. wait for client data...\n" );

    // receive remote data
    sockaddr_in remote_addr;
    bzero( &remote_addr, sizeof remote_addr );
    socklen_t remote_len = sizeof(remote_addr);

    char recv_data[100] = {0};
    ssize_t ret;
    ret = recvfrom( server_skt, recv_data, sizeof recv_data, 0, (sockaddr*)&remote_addr, &remote_len );

    if( ret > 0 )   
    {
        printf( "recv from ip = %s, port = %d\n", inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port) );
        printf( "recv data. ret = %ld, recv_data = %s\n", ret, recv_data );
    }
    else
    {
        printf( "ret = %ld, error\n", ret );
#ifdef _WIN32
        closesocket( server_skt );
        WSACleanup();
#elif defined(UNIX) || defined(MACOS)
        close( server_skt );
#endif
        return;
    }

    // send back
    char send_data[100] = "hello, this is server.\n";
    ret = sendto( server_skt, send_data, strlen(send_data), 0, (sockaddr*)&remote_addr, remote_len );
    printf( "send back. ret = %ld\n", ret );

#ifdef _WIN32
    closesocket(server_skt);
    WSACleanup();
#elif defined(UNIX) || defined(MACOS)
    close(server_skt);
#endif
}




void udp_package_order_server()
{
#ifdef _WIN32
    // windows need init
    WORD socket_version = MAKEWORD(2,2);
    WSADATA wsa_data; 
    if( WSAStartup( socket_version, &wsa_data ) != 0 )
    {
        printf("init error\n");
        return;
    }
#endif

    int ret = 0;
    SOCKET server_skt = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP );
    
    sockaddr_in local_addr;
    bzero( &local_addr, sizeof local_addr );
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons( 3891 );
    local_addr.sin_addr.s_addr = INADDR_ANY;
    int local_len = sizeof local_addr;
    
    ret = bind( server_skt, (sockaddr *)&local_addr, local_len );
    if( ret == SOCKET_ERROR )
    {
        printf( "bind fail.\n" );
        return;
    }
    
    // start recv data
    sockaddr_in remote_addr;
    socklen_t remote_len = sizeof remote_addr;
    char *recv_buf = new char[sizeof(OrderData)];
    ssize_t recv_ret;
    
    OrderData od;
    int last_order = -1;
    int cc = 0;
    int total = 0, miss = 0;
    
    while(true)
    {
        total++;
        recv_ret = recvfrom( server_skt, recv_buf, sizeof(OrderData), 0, 
                             (sockaddr *)&remote_addr, &remote_len );
        
        if( recv_ret < 0 )
        {
            printf("recv end.\n");
            break;
        }
        
        // 理論上可以不用複製,不過通常用途會需要複製資料出來,讓socket可以收其他資料
        memcpy( &od, recv_buf, sizeof(OrderData) );
        
        if( od.order_index - last_order != 1 )
        {
            miss++;
            printf("\norder fall od.order_index = %d, last_order = %d!!\n", od.order_index, last_order );
            printf("miss rate = %d/%d = %lf\n", miss, total, 1.0*miss/total );
        }
        last_order = od.order_index;
        
        //printf( "index = %d\n", od.order_index );
        if( cc++ % 10000 == 0 )
        {
            printf(".");
            fflush(stdout);
        }
    }
                 
    delete [] recv_buf;
    recv_buf = nullptr;
}



void udp_package_order_client()
{
#ifdef _WIN32
    // windows need init
    WORD socket_version = MAKEWORD(2,2);
    WSADATA wsa_data; 
    if( WSAStartup( socket_version, &wsa_data ) != 0 )
    {
        printf("init error\n");
        return;
    }
#endif

    SOCKET skt = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP );
    
    sockaddr_in remote_addr;
    bzero( &remote_addr, sizeof remote_addr );
    
    remote_addr.sin_port = htons( 3891 );
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_addr.s_addr = inet_addr( "1.163.100.76" );
    
    int remote_len = sizeof remote_addr;
    
    OrderData od;
    od.order_index = 0;
    sprintf( od.filling, "This is client message. index = %d.", od.order_index );
    
    char *send_buf = new char[sizeof(OrderData)];
    ssize_t send_ret;
    
    while(true)
    {
        memcpy( send_buf, &od, sizeof(OrderData) );
        send_ret = sendto( skt, send_buf, sizeof(OrderData), 0, (sockaddr*)&remote_addr, remote_len );
        
        if( send_ret < 0 )
        {
            printf("send fail\n");
            break;
        }
        
        od.order_index++;
        sprintf( od.filling, "This is client message. index = %d.", od.order_index );

        printf("send index = %d\n", od.order_index );
    }
    
    delete [] send_buf;
    send_buf = nullptr;

#ifdef _WIN32
    closesocket( skt );
    WSACleanup();
#elif defined(UNIX) || defined(MACOS)
    close( skt );
#endif
}
