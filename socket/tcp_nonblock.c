#include "tcp_nonblock.h"

#include <stdio.h>
#include <WinSock2.h>


#ifdef _WIN32
typedef int socklen_t;
#endif




/*
這邊的client採用blocking socket設計.
*/
void tcp_client_non_blocking( const char* const ip, int port )
{
    WORD ws_ver = MAKEWORD(2,2);
    WSADATA wsa_data;
    if( WSAStartup( ws_ver, &wsa_data ) != 0 )
    {
        printf("init fail.\n");
        return;
    }

    SOCKET skt = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
}






int tcp_init_socket()
{
#ifdef _WIN32
    WORD ws_ver = MAKEWORD(2,2);
    WSADATA wsa_data;
    if( WSAStartup(ws_ver, &wsa_data) != 0 )
    {
        printf("init error");
        return -1;
    }
#endif
}



int tcp_set_nonblocking( SOCKET skt )
{
    int res;

    /* 
    If blcok_mode = 0, blocking is enabled; 
    If blcok_mode != 0, non-blocking mode is enabled.
    */
    u_long block_mode = 1;
    res = ioctlsocket( skt, FIONBIO, &block_mode );
    if( res != NO_ERROR )
    {
        int err = WSAGetLastError();
        printf("set non-blocking fail. res = %d, err = %d\n", res, err );
        return -1;
    }

    return 1;
}



SOCKET tcp_get_max_socket( fd_set set )
{
    SOCKET max = INVALID_SOCKET;
    for( int i = 0; i < set.fd_count; i++ )
    {
        if( max < set.fd_array[i] )
            max = set.fd_array[i];
    }
    return max;
}



void tcp_server_non_blocking( int port )
{
    int res;
    res = tcp_init_socket();
    if( res < 0 )
        return;

    SOCKET listen_skt = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
    if( listen_skt == INVALID_SOCKET )
    {
        printf("socket error !");
        return;
    }

    res = tcp_set_nonblocking(listen_skt);
    if( res < 0 )
        return;

    //
    struct sockaddr_in sin;
    socklen_t sin_len = sizeof(sin);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = INADDR_ANY; 

    res = bind( listen_skt, (struct sockaddr*)&sin, sin_len );
    if( res == SOCKET_ERROR )
    {
        printf( "socket bind error!" );
        return;
    }

    //
    res = listen( listen_skt, SOMAXCONN );
    if( res == SOCKET_ERROR )
    {
        printf("listen fai...\n");
        return;
    }

    fd_set fd_socket;
    FD_ZERO( &fd_socket );
    FD_SET( listen_skt, &fd_socket );

    // 測試用,這邊先不加入write set.
    fd_set fd_read; //, fd_write;
    char buffer[1024];
    SOCKET max_skt = tcp_get_max_socket(fd_socket);

    int ret;
    while(1)
    {
        FD_ZERO( &fd_read );
        fd_read = fd_socket;

        //FD_ZERO( &fd_write );
        //fd_write = fd_socket;

        // can set timeout by parameter 5
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 500000;
        //ret = select( 0, &fd_read, &fd_write, NULL, NULL ); 
        ret = select( max_skt+1, &fd_read, NULL, NULL, &timeout ); 

        if( ret < 0 )
        {
            printf("select fail. ret = %d, break.\n", ret );
            break;
        }
        else if( ret == 0 )        
            printf("time out. ret = %d\n" );        

        for( int i = 0; i < (int)fd_socket.fd_count; ++i )
        {
            if ( FD_ISSET( fd_socket.fd_array[i], &fd_read) )
            {
                if( fd_socket.fd_array[i] == listen_skt )
                {
                    struct sockaddr_in remote;
                    socklen_t remote_len = sizeof(remote);
                    SOCKET skt = accept( listen_skt, (struct sockaddr*)&remote, &remote_len);
                    res = tcp_set_nonblocking(skt);
                    if( res < 0 )
                    {
                        printf("set non-blocking fail. skt = %d.\n", res );
                        return;
                    }
                    FD_SET( skt, &fd_socket );
                    printf("a new client connected! from ip = %s, port = %d...\n", inet_ntoa(remote.sin_addr), ntohs(remote.sin_port) );
                }
                else
                {
                    memset( buffer, 0, 1024 );
                    ret = recv( fd_socket.fd_array[i], buffer, 1024, 0 );

                    if( ret > 0 )
                    {
                        printf("recv. ret = %d. from client %d, msg = %s\n", ret, fd_socket.fd_array[i], buffer );
                        sprintf( buffer, "Hi. This is server's response. Nice to meet you.\n" );                        
                        ret = send( fd_socket.fd_array[i], buffer , strlen(buffer), 0 );
                        printf( "send. ret = %d\n", ret );
                        if( ret <= 0 )
                        {
                            closesocket( fd_socket.fd_array[i] );
                            FD_CLR( fd_socket.fd_array[i], &fd_socket );
                        }
                    }
                    else
                    {
                        printf("ret = %d\n", ret );
                        closesocket( fd_socket.fd_array[i] );
                        FD_CLR( fd_socket.fd_array[i], &fd_socket );
                    }
                }
            }
        }
    }

    closesocket(listen_skt);
    WSACleanup();
}




