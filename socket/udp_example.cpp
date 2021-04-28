#include "udp_example.h"
#include <stdio.h>
#include <string>
#include <string.h>

#ifdef _WIN32
#include <WinSock2.h>
#elif defined(UNIX) || defined(MACOS)
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <arpa/inet.h>
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



void udp_bind_client_socket( int skt )
{
    // 用底下的bind可以做到指定client port的效果,但不設定也會自動設定port.
    sockaddr_in local_addr;
    bzero(&local_addr, sizeof local_addr);  // windows沒有bzero
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(24256);  // local port指定24256

#ifdef _WIN32
    local_addr.sin_addr.S_un.S_addr = INADDR_ANY; // inet_addr("111.248.195.94");
#elif defined(UNIX)
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
#elif defined(UNIX)
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
#elif defined(UNIX)
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
#elif defined(UNIX)
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
#elif defined(UNIX)
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
#elif defined(UNIX)
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
#elif defined(UNIX)
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
#elif defined(UNIX)
    close( skt );
#endif
}
