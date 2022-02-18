#include "tcp_example_2.h"


#ifdef _WIN32
#include <WinSock2.h>
#else
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>



#ifdef _WIN32
typedef int socklen_t;
typedef int ssize_t;
#else
typedef int SOCKET;
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#endif





void tcp_hello_server_2( int port )
{
    srand( (unsigned char)time(0) );

#ifdef _WIN32
    WORD winsock_version = MAKEWORD(2,2);
    WSADATA wsa_data;
    if(WSAStartup(winsock_version, &wsa_data)!=0)
    {
        printf("init error\n");
        return;
    }
#endif

    SOCKET listen_skt = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
    if( listen_skt == INVALID_SOCKET )
    {
        printf( "socket error ! listen_skt = %d\n", (int)listen_skt );
        return;
    }

    //
    struct sockaddr_in local_addr;
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(port);
#ifdef _WIN32
    local_addr.sin_addr.S_un.S_addr = INADDR_ANY; 
#else
    local_addr.sin_addr.s_addr = INADDR_ANY;
#endif

    // note: linux not support LPSOCKADDR, need defined it.
    int res = bind( listen_skt, (struct sockaddr*)&local_addr, sizeof(local_addr) );  // LPSOCKADDR 在window下可以取代 sockaddr*
    if( res == SOCKET_ERROR)   
    {
#ifdef _WIN32
        int err = WSAGetLastError();
#else
        int err = errno;
#endif
        printf( "bind error! res = %d, err = %d\n", res, err );
        return;
    }

    res = listen( listen_skt, SOMAXCONN );   // listen 的第二個傳入變數代表的是最高連線數.   so max conn 代表預設最大連線數
    if( res  == SOCKET_ERROR )
    {
#ifdef _WIN32
        int err = WSAGetLastError();
#else
        int err = errno;
#endif
        printf( "listen error ! res = %d, err = %d\n", res, err );
        return;
    }

    //
    SOCKET client_skt;
    struct sockaddr_in remote_addr;
    socklen_t remote_addr_len = sizeof(remote_addr);
    char recv_buf[255]; 
    char send_buf[255];

    //
    int round_size = rand() % 100 + 100;
    ssize_t ret;
    printf("round size = %d\n", round_size);

    for( int i = 0; i < round_size; i++ )
    {
        client_skt = accept( listen_skt, (struct sockaddr*)&remote_addr, &remote_addr_len );
        if( client_skt == INVALID_SOCKET )
        {
#ifdef _WIN32
            int err = WSAGetLastError();
            closesocket(client_skt);
#else
            int err = errno;
            close(client_skt);
#endif
            printf( "accept error ! client_skt = %d, err = %d\n", (int)client_skt, err );
            break;
        }   
        printf("accept from : %s, port = %d", inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port) );

        int rs_size = rand() % 10 + 3;   // rs = recv, send.
        printf("recv send size = %d\n", rs_size );
        for( int j = 0; j < rs_size; )
        {
            // recv.
            memset( recv_buf, 0, 255 );
            ret = recv( client_skt, recv_buf, 255, 0 );
            if( ret == SOCKET_ERROR )
            {
#ifdef _WIN32
                int err = WSAGetLastError();
#else
                int err = errno;
#endif
                printf( "ret = %d. break. err = %d\n", (int)ret, err );
                break;
            }
            else if( ret == 0 )
            {
                printf("recv. remote close socket. ret = %d\n", (int)ret);
                break;
            }
            else
                printf("ret = %d. recv msg = %s\n", (int)ret, recv_buf );

            // 希望有機會在send or recv的時候中斷. 所以recv後 j++
            j++;
            if( j >= rs_size )
                break;

            memset( send_buf, 0, 255 );
            sprintf( send_buf, "This is server. round = %d, recv send %d\n", i, j );
            ret = send( client_skt, send_buf, strlen(send_buf), 0 );
            if( ret == SOCKET_ERROR )
            {
#ifdef _WIN32
                int err = WSAGetLastError();
#else
                int err = errno;
#endif
                printf("ret = %d, break. err = %d\n", (int)ret, err );
                break;
            }
            else if( ret == 0 )
            {
                printf( "send. remote close socket, ret = %d\n", (int)ret );
                break;
            }
            else
                printf("send. ret = %d\n", (int)ret);
        }

        // need close skt. release source.
#ifdef _WIN32
        closesocket( client_skt );
#else
        close(client_skt);
#endif
    }

    printf("round size = %d\n", round_size);


    // 
#ifdef _WIN32
    closesocket(listen_skt);
    WSACleanup();
#else
    close(listen_skt);
#endif

    return;
}






void tcp_hello_client_2( const char *ip, int  port )
{
    srand( (unsigned int)time(0) );

#ifdef _WIN32
    WORD sock_version = MAKEWORD(2,2);
    WSADATA wsa_data; 
    if( 0 != WSAStartup(sock_version, &wsa_data) )
    {
        printf("init error\n");
        return;
    }
#endif

    struct sockaddr_in remote_addr;
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(port);
#ifdef _WIN32
    remote_addr.sin_addr.S_un.S_addr = inet_addr(ip); 
#else
    remote_addr.sin_addr.s_addr = inet_addr(ip);
#endif

    //
    int round_size = rand() % 100 + 100;
    ssize_t ret;
    char recv_buf[255];
    char send_buf[255];
    SOCKET skt = INVALID_SOCKET;

    printf( "round size = %d\n", round_size );


    for( int i = 0; i < round_size; i++ )
    {
        skt = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
        if( skt == INVALID_SOCKET )
        {
            printf( "invalid socket! skt = %d\n", (int)skt );
            return;
        }
    
        printf("wait connect...\n");
        ret = connect( skt, (struct sockaddr *)&remote_addr, sizeof(remote_addr) );
        if( ret == SOCKET_ERROR )
        {
#ifdef _WIN32
            int err = WSAGetLastError();
            closesocket(skt);
#else
            int err = errno;
            close(skt);
#endif
            printf("connect error! res = %d, err = %d\n", (int)ret, err );
            break;
        }
        printf("connect success.\n");       

        int rs_size = rand() % 10 + 3;
        printf( "recv send size = %d\n", rs_size );

        for( int j = 0; j < rs_size; )
        {
            memset( send_buf, 0, 255 );
            sprintf( send_buf, "Hello, this is client. round = %d, recv send = %d\n", i, j );
            ret = send( skt, send_buf, (int)strlen(send_buf), 0 );
            if( ret == SOCKET_ERROR )
            {
#ifdef _WIN32
                int err = WSAGetLastError();
#else
                int err = errno;
#endif
                printf("send error. ret = %d, err = %d\n", (int)ret, err );
                break;
            }
            else if( ret == 0 )
            {
                printf("send. remote close. ret = %d\n", (int)ret );
                break;
            }
            else
                printf("send. ret = %d\n", (int)ret );

            // 希望有機會send或recv時中斷,所以這邊需要j++
            j++;
            if( j >= rs_size )
                break;

            memset( recv_buf, 0, 255 );
            ret = recv( skt, recv_buf, 255, 0 );
            if( ret == SOCKET_ERROR )
            {
#ifdef _WIN32
                int err = WSAGetLastError();
#else
                int err = errno;
#endif
                printf("recv error. ret = %d, err = %d\n", (int)ret, err );
                break;
            }
            else if( ret == 0 )
            {
                printf("recv remote close. ret = %d\n", (int)ret );
                break;
            }
            else
                printf("recv. ret = %d, msg = %s\n", (int)ret, recv_buf );

            printf("recv message. ret = %d, msg = %s\n", (int)ret, recv_buf );
            
            //
            j++;
        }
        
#ifdef _WIN32
        closesocket(skt);
#else
        close(skt);
#endif
    }
    
    printf( "round size = %d\n", round_size );

#ifdef _WIN32
    WSACleanup();
#else
    close(skt);
#endif

    return ;
}


