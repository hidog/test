#include "udp_example_2.h"

#include <string.h>

#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

// 使用qtcreator的時候報錯, flag設c11雖然能解決,卻遇到其他錯誤,所以最後直接定義
#ifndef NULL
#define NULL 0
#endif



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



/*
    為了方便, 用同一個function處理server, client
    ip == NULL 代表 server, 反之 client
*/
int get_udp_socket( const char* ip, const char* port )
{
    int ret, skt;
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
        printf(  "gai_strerror: %s\n", gai_strerror(ret) );
        return -1;
    }

    // 用迴圈來找出全部的結果，並 bind 到首先找到能 bind 的
    for( ptr = servinfo; ptr != NULL; ptr = ptr->ai_next )
    {
        skt = socket( ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol );

        if ( skt == -1 )
        {
            printf("next skt...\n");
            continue;
        }

        if( ip == NULL )
            ret = bind( skt, ptr->ai_addr, ptr->ai_addrlen );
        else
            ret = connect( skt, ptr->ai_addr, ptr->ai_addrlen );
            /*
                ip 有資料, 給client端用的
                因為client需要 ptr->ai_addr, ptr->ai_addrlen才能做 send_to的動作
                為了寫作方便, 改成connect的設計.
                如果要用sendto, 則需要回傳 ptr 資訊回去,並且不能freeaddinfo
            */

        if( ret == -1 )
        {
            close(skt);
            printf("bind or connect next skt...\n");
            continue;
        }

        printf( "bind or connect success, skt = %d\n", skt );
        break;
    }

    if( ptr == NULL )
    {
        printf( "listener: failed to bind or connect socket\n" );
        return -1;
    }

    freeaddrinfo(servinfo);

    return skt;
}






void udp_hello_server_2( const char* port )
{
    /*int numbytes;

    char buf[200];

    char str[INET6_ADDRSTRLEN];*/

    struct sockaddr_storage remote_addr;
    socklen_t remote_len;
    int skt = get_udp_socket( NULL, port );
    int ret;
    char recv_buf[200] = {0};

    printf( "listener: waiting to recvfrom...\n" );
    remote_len = sizeof remote_addr;

    ret = recvfrom( skt, recv_buf, 200, 0, (struct sockaddr *)&remote_addr, &remote_len );
    if( ret == -1 )
    {
        printf( "recv error, ret = %d\n", ret );
        return;
    }

    char ip_str[INET6_ADDRSTRLEN];
    void *addr_ptr = get_in_addr( (struct sockaddr *)&remote_addr );
    int remote_port = get_in_port( (struct sockaddr *)&remote_addr );

    // 底下兩者作用一樣.
    inet_ntop( remote_addr.ss_family, addr_ptr, ip_str, sizeof ip_str );
    //strcpy( ip_str, inet_ntoa( ((struct sockaddr_in *)&remote_addr)->sin_addr) );

    printf( "listener: got packet from %s, port = %d\n", ip_str, remote_port );
    printf( "listener: packet is %d bytes long\n", ret );
    printf( "listener: msg : %s\n", recv_buf );

    // send back
    char send_buf[100] = "This is server. response your message.";
    ret = sendto( skt, send_buf, strlen(send_buf), 0, (struct sockaddr *)&remote_addr, remote_len );
    if( ret < 0 )
    {
        printf("error. ret = %d\n", ret );
        return;
    }
    else
        printf("send back success, ret = %d\n", ret );


    close(skt);
}




void udp_hello_client_2( const char* ip, const char* port )
{
    int skt = get_udp_socket( ip, port );

    // send
    char send_buf[100] = "Hello, this is client\n";
    int ret = write( skt, send_buf, strlen(send_buf) );

    if( ret < 0 )
    {
        printf("send fail...\n");
        return;
    }
    else
        printf("send success!! ret = %d\n", ret );

    // recv
    char recv_buf[100] = {0};
    ret = read( skt, recv_buf, 100 );

    if( ret < 0 )
    {
        printf("recv fail...\n");
        return;
    }
    else
        printf("recv from server. ret = %d, msg = %s\n", ret, recv_buf );

    close(skt);
}
