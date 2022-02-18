#include "udp_example_2.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#if defined(UNIX) || defined(MACOS)
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#elif defined(_WIN32)
#include <WinSock2.h>
#include <WS2tcpip.h>   // for sockaddr_in6
#endif


// 使用qtcreator的時候報錯, flag設c11雖然能解決,卻遇到其他錯誤,所以最後直接定義
#ifndef NULL
#define NULL 0
#endif


#ifdef _WIN32
typedef int socklen_t;
typedef int ssize_t;
#define bzero(ptr,size)     memset( (ptr), 0, (size) )
#elif defined(UNIX) || defined(MACOS)
typedef int SOCKET;
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#endif



struct MultiData
{
    int identity;
    char message[200];
};




struct LossData
{
    int index;
    char filling[1490];
};


// get sockaddr, IPv4 or IPv6:
// sockaddr_in 沒有sa_family欄位
static void* get_in_addr( struct sockaddr *addr )
{
    if( addr->sa_family == AF_INET )
    {
        struct sockaddr_in* ptr = (struct sockaddr_in *)addr;
        return &( ptr->sin_addr );
    }
    else
    {
        struct sockaddr_in6* ptr = (struct sockaddr_in6 *)addr;
        return &( ptr->sin6_addr );
    }
}


static int get_in_port( struct sockaddr *addr )
{
    if( addr->sa_family == AF_INET )
    {
        struct sockaddr_in* ptr = (struct sockaddr_in *)addr;
        return ntohs( ptr->sin_port );
    }
    else
    {
        struct sockaddr_in6* ptr = (struct sockaddr_in6 *)addr;
        return ntohs( ptr->sin6_port );
    }
}






void udp_nonblocking_2(void)
{
    srand( (unsigned char)time(NULL) );
    
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
    struct sockaddr_in local_addr;
    SOCKET send_skt, recv_skt;
    SOCKET max_skt = -1;
    int flag;

#ifdef _WIN32
    u_long block_mode = 1;   // zero = blocking, not zero = non-blocking
#endif

    // 一般來講,需要檢查socket是否create成功.
    send_skt = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP );
    recv_skt = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP );
    max_skt = send_skt > recv_skt ? send_skt : recv_skt;
    
#ifdef _WIN32
    // note: 一般來講應該要檢查設置是否成功
    ioctlsocket( send_skt, FIONBIO, &block_mode );
    ioctlsocket( recv_skt, FIONBIO, &block_mode );
#else
    // note: 一般來講應該要檢查設置是否成功.
    fcntl( send_skt, F_SETFL, fcntl(send_skt,F_GETFL,0) | O_NONBLOCK ); 
    fcntl( recv_skt, F_SETFL, fcntl(send_skt,F_GETFL,0) | O_NONBLOCK ); 
#endif
    
    //
    bzero( &local_addr, sizeof local_addr );
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
 
    //
    local_addr.sin_port = htons( 1234 );
    flag = bind( send_skt, (struct sockaddr*)&local_addr, sizeof local_addr );
    if( flag == SOCKET_ERROR )
    {
        printf("bind send skt fail\n");
        return;
    }
    
    //
    local_addr.sin_port = htons( 1235 );
    flag = bind( recv_skt, (struct sockaddr*)&local_addr, sizeof local_addr );
    if( flag == SOCKET_ERROR )
    {
        printf("bind send skt fail\n");
        return;
    }
    
    printf( "bind success. wait for client data...\n" );

    // receive remote data
    struct sockaddr_in remote_addr;
    socklen_t remote_len = sizeof(remote_addr);
    
    char ip_list[9][100] = { "192.168.1.106",  // imac
                             "192.168.3.240",  // room
                             "192.168.3.191",  // 2F
                             "192.168.1.110",  // ubuntu red
                             "192.168.1.108",  // ubuntu
                             "192.168.2.247",  // x250
                             "192.168.2.116",  // mbpr
                             "192.168.1.102",  // slave
                             "192.168.1.101"   // master
                           };
    
    //
    fd_set read_set, write_set;
    struct timeval timeout = { 2, 500000 };
    char recv_buf[1400], send_buf[1400];
    char *my_ip = "192.168.1.101";
    ssize_t ret;
    int target;
    
    while(1)
    {
        target = rand() % 18;
        
        FD_ZERO( &read_set );
        FD_ZERO( &write_set );
        
        FD_SET( send_skt, &write_set );
        FD_SET( recv_skt, &read_set );
        
        flag = select( (int)(max_skt+1), &read_set, &write_set, 0, &timeout );
        if( flag == SOCKET_ERROR )
        {
            printf("select error\n");
            break;
        }
        
        if( FD_ISSET( recv_skt, &read_set ) )
        {
            ret = recvfrom( recv_skt, recv_buf, 1400, 0, (struct sockaddr*)&remote_addr, &remote_len );
            if( ret != 1400 )
            {
                printf("recv ret != 1400. error!!\n");
                break;
            }
            printf( "recv msg = %s\n", recv_buf );
        }
        
        if( FD_ISSET( send_skt, &write_set ) )
        {
            if( target < 9 )
            {
#ifdef MACOS
                sprintf( send_buf, "I am %s, my ip is %s", "MACOS", my_ip );
#elif defined(_WIN32)
                sprintf( send_buf, "I am %s, my ip is %s", "WINDOWS", my_ip );
#elif defined(UNIX)
                sprintf( send_buf, "I am %s, my ip is %s", "UBUNTU", my_ip );
#endif
                
                bzero( &remote_addr, sizeof(remote_addr) );
                remote_addr.sin_family = AF_INET;
                remote_addr.sin_addr.s_addr = inet_addr( ip_list[target] );
                remote_addr.sin_port = htons(1235);
                
                ret = sendto( send_skt, send_buf, 1400, 0, (struct sockaddr*)&remote_addr, remote_len );
                if( ret != 1400 )
                {
                    printf("send ret != 1400, error!!\n");
                    break;
                }
                printf("send to %s, ret = %d\n", ip_list[target], (int)ret );
            }
        }
        printf("\n");
#ifdef _WIN32
        Sleep(1000);
#else
        sleep(1);
#endif
    }

#ifdef _WIN32
    closesocket(send_skt);
    closesocket(recv_skt);
    WSACleanup();
#elif defined(UNIX) || defined(MACOS)
    close(send_skt);
    close(recv_skt);
#endif 
    
}







void udp_multi_client(void)
{
    srand( (unsigned int)time(NULL) );

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
    struct sockaddr_in remote_addr;
    bzero( &remote_addr, sizeof remote_addr );
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(38871);
#ifdef _WIN32
    remote_addr.sin_addr.S_un.S_addr = inet_addr( "122.116.84.59" );  
#else
    remote_addr.sin_addr.s_addr = inet_addr( "122.116.84.59" );
#endif
    socklen_t remote_len = sizeof(remote_addr);

    //
    struct MultiData multi_data;
    const int multi_data_size = sizeof( struct MultiData );
    char *send_buf = (char*)malloc(multi_data_size);
    if( send_buf == NULL )
    {
        printf("malloc fail.\n");
        return;
    }

    multi_data.identity = rand() % 10000;
    ssize_t ret;

    char recv_buf[300];

    while(1)
    {
#ifdef _WIN32
        sprintf( multi_data.message, "Hello, this is windows" );
#elif defined(UNIX)
        sprintf( multi_data.message, "Hello, this is ubuntu" );
#elif defined(MACOS)
        sprintf( multi_data.message, "Hello, this is MACOS" );
#endif
        memcpy( send_buf, &multi_data, multi_data_size );
        ret = sendto( client_skt, send_buf, multi_data_size, 0, (struct sockaddr*)&remote_addr, remote_len );
        if( ret < 0 )
        {
            printf("send fail.\n");
            break;
        }
        printf("send to server. ret = %d\n", (int)ret );

        ret = recvfrom( client_skt, recv_buf, 300, 0, (struct sockaddr*)&remote_addr, &remote_len );
        if( ret < 0 )
        {
            printf("recv fail.\n");
            break;
        }

        printf("recv from server. ret = %d, msg = %s\n", (int)ret, recv_buf );
        printf( "\n" );

#ifdef _WIN32
        Sleep(1000);
#else
        sleep(1);
#endif
    }

    free(send_buf);
    send_buf = NULL;

#ifdef _WIN32
    closesocket( client_skt );
    WSACleanup();
#elif defined(UNIX) || defined(MACOS)
    close(client_skt);
#endif
}



void udp_multi_server(void)
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

    struct sockaddr_in local_addr;
    bzero( &local_addr, sizeof local_addr );  // windows沒有bzero
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(38871);

#ifdef _WIN32
    local_addr.sin_addr.S_un.S_addr = INADDR_ANY;
#else
    local_addr.sin_addr.s_addr = INADDR_ANY; 
#endif
    int local_len = sizeof(local_addr);

    // https://man7.org/linux/man-pages/man2/bind.2.html   see how to handle error
    if( bind(server_skt, (struct sockaddr*)&local_addr, local_len ) == SOCKET_ERROR )
    {
        printf("bind error !\n");
        return;
    }

    printf( "bind success. wait for client data...\n" );

    // receive remote data
    struct sockaddr_in remote_addr;
    bzero( &remote_addr, sizeof remote_addr );
    socklen_t remote_len = sizeof(remote_addr);

    char send_data[100] = {"hello, this is server."};

    const int multi_data_size = sizeof(struct MultiData);
    char *recv_buf = (char*)malloc(multi_data_size);
    if( recv_buf == NULL )
    {
        printf("malloc error\n");
        return;
    }
    ssize_t ret;

    struct MultiData multi_data;

    int id_map[100];
    int max_user = 0;
    for( int i = 0; i < 100; i++ )
        id_map[i] = -1;
    int find_in_map;

    while(1)
    {
        ret = recvfrom( server_skt, recv_buf, multi_data_size, 0, (struct sockaddr*)&remote_addr, &remote_len );

        if( ret > 0 )   
        {
            memcpy( &multi_data, recv_buf, multi_data_size );
            find_in_map = 0;
            for( int i = 0; i <= max_user; i++ )
            {
                if( id_map[i] == multi_data.identity )
                {
                    find_in_map = 1;
                    break;
                }
            }
            if( find_in_map == 0 )
            {
                id_map[max_user] = multi_data.identity;
                max_user++;
            }

            printf( "max user = %d\n", max_user );
            printf( "recv from ip = %s, port = %d\n", inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port) );
            printf( "recv data. ret = %ld, id = %d, message = %s\n", ret, multi_data.identity, multi_data.message );
            printf( "\n" );
        }
        else
        {
            printf( "ret = %ld, error\n", ret );
            break;
        }

        // send back
        ret = sendto( server_skt, send_data, 100, 0, (struct sockaddr*)&remote_addr, remote_len );
        printf( "send back. ret = %ld\n", ret );
    }

    free(recv_buf);
    recv_buf = NULL;

#ifdef _WIN32
    closesocket(server_skt);
    WSACleanup();
#elif defined(UNIX) || defined(MACOS)
    close(server_skt);
#endif
}




/*
    為了方便, 用同一個function處理server, client
    ip == NULL 代表 server, 反之 client
*/
#ifdef _WIN64
unsigned __int64 get_udp_socket( const char* ip, const char* port )
#else
int get_udp_socket( const char* ip, const char* port )
#endif
{
    int ret;
    SOCKET skt = 0;
    struct addrinfo hints, *servinfo, *ptr;

    memset(&hints, 0, sizeof hints);

    hints.ai_family     =   AF_UNSPEC; // 設定 AF_INET 以強制使用 IPv4
    hints.ai_socktype   =   SOCK_DGRAM;

    if( ip == NULL ) // means client code
        hints.ai_flags  =   AI_PASSIVE; // 使用我的 IP

    // if ip == NULL, means use by server side
    ret = getaddrinfo( ip, port, &hints, &servinfo );
    if( ret != 0 )
    {
#ifdef UNICODE
        printf(  "gai_strerror: %ws\n", gai_strerror(ret) );
#else
        printf("gai_strerror: %s\n", gai_strerror(ret));
#endif
        return -1;
    }

    // 用迴圈來找出全部的結果，並 bind 到首先找到能 bind 的
    for( ptr = servinfo; ptr != NULL; ptr = ptr->ai_next )
    {
#ifdef _WIN32
        // windows平台會綁到ip6, 只好加workaround
        if( ptr->ai_family == AF_INET6 )
            continue;
#endif

        skt = socket( ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol );

        if ( skt == -1 )
        {
            printf("next skt...\n");
            continue;
        }

        if( ip == NULL )
            ret = bind( skt, ptr->ai_addr, (int)ptr->ai_addrlen );
        else
            ret = connect( skt, ptr->ai_addr, (int)ptr->ai_addrlen );
            /*
                ip 有資料, 給client端用的
                因為client需要 ptr->ai_addr, ptr->ai_addrlen才能做 send_to的動作
                為了寫作方便, 改成connect的設計.
                如果要用sendto, 則需要回傳 ptr 資訊回去,並且不能freeaddinfo
            */

        if( ret == -1 )
        {
#ifdef _WIN32
            closesocket(skt);
#elif defined(UNIX) || defined(MACOS)
            close(skt);
#endif
            printf("bind or connect next skt...\n");
            continue;
        }

        printf( "bind or connect success, skt = %d\n", (int)skt );
        break;
    }

    if( ptr == NULL )
    {
        printf( "listener: failed to bind or connect socket\n" );
        return -1;
    }

    freeaddrinfo(servinfo);

    return skt;  // 使用int的話, 在windows 64出現warning
}






void udp_hello_server_2( const char* port )
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

    struct sockaddr_storage remote_addr;
    socklen_t remote_len;
    SOCKET skt = get_udp_socket( NULL, port );
    ssize_t ret;
    char recv_buf[200] = {0};

    printf( "listener: waiting to recvfrom...\n" );
    remote_len = sizeof remote_addr;

    ret = recvfrom( skt, recv_buf, 200, 0, (struct sockaddr *)&remote_addr, &remote_len );
    if( ret == -1 )
    {
        printf( "recv error, ret = %ld\n", ret );
        return;
    }

    char ip_str[INET6_ADDRSTRLEN];
    void *addr_ptr = get_in_addr( (struct sockaddr *)&remote_addr );
    int remote_port = get_in_port( (struct sockaddr *)&remote_addr );

    // 底下兩者作用一樣.
    inet_ntop( remote_addr.ss_family, addr_ptr, ip_str, sizeof ip_str );
    //strcpy( ip_str, inet_ntoa( ((struct sockaddr_in *)&remote_addr)->sin_addr) );

    printf( "listener: got packet from %s, port = %d\n", ip_str, remote_port );
    printf( "listener: packet is %d bytes long\n", (int)ret );
    printf( "listener: msg : %s\n", recv_buf );

    // send back
    char send_buf[100] = {"This is server. response your message."};
    ret = sendto( skt, send_buf, (int)strlen(send_buf), 0, (struct sockaddr *)&remote_addr, remote_len );
    if( ret < 0 )
    {
        printf("error. ret = %ld\n", ret );
        return;
    }
    else
        printf("send back success, ret = %d\n", (int)ret );

#ifdef _WIN32
    closesocket( skt );
    WSACleanup();
#elif defined(UNIX) || defined(MACOS)
    close(skt);
#endif

}




void udp_hello_client_2( const char* ip, const char* port )
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

    SOCKET skt = get_udp_socket( ip, port );

    // send
    char send_buf[100] = {"Hello, this is client\n"};
#ifdef _WIN32
    int ret = send( skt, send_buf, (int)strlen(send_buf), 0 );
#else
    ssize_t ret = write( skt, send_buf, strlen(send_buf) );
#endif

    if( ret < 0 )
    {
        printf("send fail...\n");
        return;
    }
    else
        printf("send success!! ret = %d\n", (int)ret );

    // recv
    char recv_buf[100] = {0};
#ifdef _WIN32
    ret = recv( skt, recv_buf, 100, 0 );
#else
    ret = read( skt, recv_buf, 100 );
#endif

    if( ret < 0 )
    {
        printf("recv fail...\n");
        return;
    }
    else
        printf("recv from server. ret = %d, msg = %s\n", (int)ret, recv_buf );

#ifdef _WIN32
    closesocket( skt );
    WSACleanup();
#elif defined(UNIX) || defined(MACOS)
    close(skt);
#endif
}





void udp_test_package_loss_server(void)
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

    struct sockaddr_in local_addr;
    bzero( &local_addr, sizeof local_addr );
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons( 7227 );
    local_addr.sin_addr.s_addr = INADDR_ANY;  // windows有定義,能work. 舊版visual studio或許不行
    int local_len = sizeof local_addr;

    ret = bind( server_skt, (struct sockaddr *)&local_addr, local_len );
    if( ret == SOCKET_ERROR )
    {
        printf( "bind fail.\n" );
        return;
    }

    // set timeout的code
    // 收到封包後才等timeout
#ifdef _WIN32
    //int timeout = 1000; // 10s
    //int set_ret = setsockopt( server_skt, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout );
    //printf("set_ret = %d\n", set_ret );
#elif defined(UNIX) || defined(MACOS)
    //struct timeval tm = { 1, 0 }; // 1s, 第二個參數是us
    //int set_ret = setsockopt( server_skt, SOL_SOCKET, SO_RCVTIMEO, &tm, sizeof tm );
    //printf("set_ret = %d\n", set_ret );
#endif

    // start recv data
    struct sockaddr_in remote_addr;
    socklen_t remote_len = sizeof remote_addr;
    char *recv_buf = (char*)malloc( sizeof(struct LossData) ); 
    if( recv_buf == NULL )
    {
        printf("malloc fail.\n");
        return;
    }

    ssize_t recv_ret;

    struct LossData ld;
    char *flag = (char*)malloc(999999); //  [999999] = {0};
    if( flag == NULL )
    {
        printf("malloc fail.\n");
        return;
    }
    memset( flag, 0, 999999 );

    int first_recv = 1;
    int set_res = 0;

    while(1)
    {
        recv_ret = recvfrom( server_skt, recv_buf, sizeof(struct LossData), 0, (struct sockaddr *)&remote_addr, &remote_len );

        if( first_recv == 1 )
        {
#ifdef _WIN32
            int timeout = 10000; // 10s
#elif defined(UNIX) || defined(MACOS)
            struct timeval timeout = { 10, 0 }; // 10s, 0us
#endif
            set_res = setsockopt( server_skt, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof timeout );
            if( set_res == SOCKET_ERROR )
            {
#ifdef _WIN32
                int err_code = WSAGetLastError();
#elif defined(UNIX) || defined(MACOS)
                int err_code = errno;
#endif
                printf("setsockopt timeout fail. res = %d, error code = %d\n", set_res, err_code );
                return;
            }

            setsockopt( server_skt, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof timeout );
            first_recv = 0;
        }

        if( recv_ret < 0 )
        {
            printf("recv end.\n");
            break;
        }

        // 理論上可以不用複製,不過通常用途會需要複製資料出來,讓socket可以收其他資料
        memcpy( &ld, recv_buf, sizeof(struct LossData) );

        flag[ld.index] = 1;
        printf( "recv index = %d\n", ld.index );
    }

    free(recv_buf);
    recv_buf = NULL;

    // check flag
    int count = 0;
    for( int i = 0; i < 999999; i++ )
    {
        if( flag[i] == 0 )
        {
            count++;
            printf("packet %d miss!!\n", i );
        }
    }
    printf("loss rate %d/%d = %lf\n", count, 999999, 1.0*count/999999 );

    free(flag);
    flag = NULL;

#ifdef _WIN32
    closesocket( server_skt );
    WSACleanup();
#elif defined(UNIX) || defined(MACOS)
    close(server_skt);
#endif
}






void udp_test_package_loss_client(void)
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
    if( skt == INVALID_SOCKET )
    {
        printf("get socket fail.\n");
        return;
    }

    struct sockaddr_in remote_addr;
    bzero( &remote_addr, sizeof remote_addr );
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons( 7227 );
    remote_addr.sin_addr.s_addr = inet_addr("122.116.84.59");
    int remote_len = sizeof remote_addr;

    // start recv data
    char *send_buf = (char*)malloc( sizeof(struct LossData) ); 
    if( send_buf == NULL )
    {
        printf("malloc fail.\n");
        return;
    }

    ssize_t send_ret;

    struct LossData ld;

    for( int i = 0; i < 999999; i++ )
    {
        ld.index = i;
        memcpy( send_buf, &ld, sizeof(struct LossData) );
        send_ret = sendto( skt, send_buf, sizeof(struct LossData), 0, (struct sockaddr *)&remote_addr, remote_len );

        if( send_ret < 0 )
        {
            printf("recv end.\n");
            break;
        }
        
        printf("index = %d, send ret = %d\n", i, (int)send_ret );

        if( i % 100 == 0 )
#ifdef _WIN32
            Sleep(100);
#elif defined(UNIX) || defined(MACOS)
            usleep(100000);
#endif

    }

    free(send_buf);
    send_buf = NULL;

#ifdef _WIN32
    closesocket( skt );
    WSACleanup();
#else
    close(skt);
#endif
}


