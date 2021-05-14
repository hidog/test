#include "tcp_nonblock.h"

#include <stdio.h>

#ifdef _WIN32
#include <WinSock2.h>
#else
#include <netinet/ip.h>
#endif



#ifdef _WIN32
typedef int socklen_t;
#endif



/*
    這邊的client採用non-blocking socket設計.    
*/
void tcp_client_non_blocking( const char* const ip, int port )
{
    tcp_init_socket();

    int res;
    SOCKET skt = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
    res = tcp_set_nonblocking(skt);
    if( res < 0 )
        return;

    struct sockaddr_in addr = tcp_setup_addr_client( ip, port );

    fd_set r_set, w_set;
    struct timeval timeout = { 1, 0 };

    /*
        non-block的狀態下, 呼叫connect高機率返回 error, 
        需要用select判斷什麼時候連上線
    */
#if 0
    // 底下這段code測試失敗,不能這樣連線
    res = connect( skt, (PSOCKADDR)&addr, sizeof(addr) );

    if( res == 0 )
        printf("connect !!\n");
    else
    {
        while(1)
        {
            int error = 0;
            socklen_t len = sizeof (error);
            int retval = getsockopt (skt, SOL_SOCKET, SO_ERROR, &error, &len);
            printf("error = %d\n", error);
            if( error == WSAEISCONN )
            {
                printf("conencted\n");
                break;
            }
            Sleep(1);
        }
    }

#elif 0
    // 這段code得到 WSAEWOULDBLOCK 的error code
    while(1)
    {
        res = connect( skt, (PSOCKADDR)&addr, sizeof(addr) );
        if( res == 0 )
        {
            printf("connect success. res = %d\n", res );
            break;
        }
        else if( res == SOCKET_ERROR )
        {
            int err = WSAGetLastError();
            if( err == WSAEINPROGRESS )
                printf("connecting...\n");
            else if( err == WSAEISCONN )
            {
                printf("connected!!\n");
                break;
            }
            else
            {
                printf("connect fail. err = %d\n", err);
                return;
            }
        }        
    }
#elif 0
    // 這段code可以運作
    res = connect( skt, (PSOCKADDR)&addr, sizeof(addr) );
    if( res == 0 )    
        printf("connect success. res = %d\n", res );    
    else
    {
        int timeout_count;
        for( timeout_count = 0; timeout_count < 10; timeout_count++ )
        { 
            FD_ZERO( &r_set );
            FD_ZERO( &w_set );

            FD_SET( skt, &r_set );
            FD_SET( skt, &w_set );

            res = select( skt+1, &r_set, &w_set, NULL, &timeout );
            if( res == SOCKET_ERROR )
            {
                printf("select fail. res = %d\n", res );
                return;
            }
            else if( res == 0 )
                printf("time out. timeout_count = %d\n", timeout_count);
            else
            {         
                // 用r_set會fail.
                if( FD_ISSET( skt, &w_set ) )
                {
                    printf("connected!!\n");
                    break;
                }
            }
        } 

        if( timeout_count == 10 )
        {
            printf("connected fail.\n");
            return;
        }
    }
#else
    // 底下這段code可以運作
    res = connect( skt, (PSOCKADDR)&addr, sizeof(addr) );
    if( res == 0 )    
        printf("connect success. res = %d\n", res );    
    else
    {
        for( int timeout_count = 0; timeout_count < 10; timeout_count++ )
        {
            FD_ZERO( &r_set );
            FD_ZERO( &w_set );

            FD_SET( skt, &r_set );
            FD_SET( skt, &w_set );

            res = select( skt+1, &r_set, &w_set, NULL, &timeout );
            if( res == SOCKET_ERROR )
            {
                printf("select fail. res = %d\n", res );
                return;
            }
            else if( res == 0 )
                printf("time out. timeout_count = %d\n", timeout_count);
            else if( FD_ISSET( skt, &r_set ) || FD_ISSET( skt, &w_set ) )
            {               
                res = connect( skt, (PSOCKADDR)&addr, sizeof(addr) );
                if( res == 0 )
                {
                    printf("connected ! res = %d\n", res );
                    break;
                }
                else if( res == SOCKET_ERROR )
                {
                    int err = WSAGetLastError();  
                    if( err == WSAEINPROGRESS )
                        printf("in progress...\n");
                    else if( err == WSAEISCONN )
                    {
                        printf("connected!\n");
                        break;
                    }
                    else
                    {
                        printf("err = %d. connect fail.\n", err );
                        return;
                    }
                }
            }
        } 
    }
#endif

    char buffer[1024];
    int count = 0;
    int ret;

    /*
        理論上應該用 w_set判斷是否能寫入
        這邊測試用,省略這個步驟.
        會造成連送兩次後才收到封包.
    */
    while(1)
    {
        FD_ZERO( &r_set );
        FD_ZERO( &w_set );

        FD_SET( skt, &r_set );
        FD_SET( skt, &w_set );

        sprintf( buffer, "Hi, this is client. non-blocking tcp test. count = %d.", count++ );
        ret = send( skt, buffer, strlen(buffer), 0 );
        if( ret == 0 )
        {
            printf("Remote closed. break. ret = %d\n", ret);
            break;
        }
        else if( ret < 0 )
        {
            printf("send fail. ret = %d\n", ret );
            break;
        }
        else
            printf("send success. ret = %d\n", ret );

        ret = select( skt+1, &r_set, &w_set, NULL, &timeout );
        switch(ret)
        {
            case SOCKET_ERROR:
                printf("select fail.\n");
                return;
            case 0:
                printf("select timeout.\n");
                break;
            default:
                //
                if( FD_ISSET( skt, &r_set ) )
                {
                    ret = recv( skt, buffer, 1024, 0 );
                    if( ret == 0 )
                    {
                        printf("remote closed. ret = %d\n", ret);
                        return;
                    }
                    else if( ret < 0 )
                    {
                        printf("recv fail. ret = %d\n", ret );
                        return;
                    }
                    else
                    {
                        if( ret < 1024 )
                            buffer[ret] = 0;
                        printf("recv ret = %d. msg = %s\n", ret, buffer );
                    }
                }
        }



        Sleep(1000);
    }

    closesocket(skt);
    WSACleanup();
}




/*
    這邊的client採用blocking socket設計.
*/
void tcp_client_blocking( const char* const ip, int port )
{
    tcp_init_socket();

    int res;
    SOCKET skt = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
    struct sockaddr_in addr = tcp_setup_addr_client( ip, port );

    res = connect( skt, (PSOCKADDR)&addr, sizeof(addr) );
    if( res == SOCKET_ERROR )
    {
        printf("connect fail. res = %d\n", res );
        return;
    }

    char buffer[1024];
    int count = 0;
    int ret;

    while(1)
    {
        sprintf( buffer, "Hi, this is client. non-blocking tcp test. count = %d.", count++ );
        ret = send( skt, buffer, strlen(buffer), 0 );
        if( ret == 0 )
        {
            printf("Remote closed. break. ret = %d\n", ret);
            break;
        }
        else if( ret < 0 )
        {
            printf("send fail. ret = %d\n", ret );
            break;
        }
        else
            printf("send success. ret = %d\n", ret );
        
        //
        ret = recv( skt, buffer, 1024, 0 );
        if( ret == 0 )
        {
            printf("remote closed. ret = %d\n", ret);
            break;
        }
        else if( ret < 0 )
        {
            printf("recv fail. ret = %d\n", ret );
            break;
        }
        else
        {
            if( ret < 1024 )
                buffer[ret] = 0;
            printf("recv ret = %d. msg = %s\n", ret, buffer );
        }
        
        Sleep(1000);
    }

    closesocket(skt);
    WSACleanup();
}




SOCKADDR_IN tcp_setup_addr_client( char *ip, int port )
{
    struct sockaddr_in sin;

    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = inet_addr(ip);

    return sin;
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


SOCKADDR_IN tcp_setup_addr_server( int port )
{
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = INADDR_ANY; 
    return sin;
}




int tcp_setup_listen_skt( SOCKET skt, int port )
{
    SOCKADDR_IN addr = tcp_setup_addr_server(port);

    socklen_t len = sizeof(addr);
    int res = bind( skt, (struct sockaddr*)&addr, len );
    if( res == SOCKET_ERROR )
    {
        printf( "socket bind error!" );
        return -1;
    }

    //
    res = listen( skt, SOMAXCONN );  // SOMAXCONN 預設最大佇列排隊數
    if( res == SOCKET_ERROR )
    {
        printf("listen fai...\n");
        return -1;
    }
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
    res = tcp_setup_listen_skt( listen_skt, port );
    if( res < 0 )
        return;

    fd_set fd_socket;
    FD_ZERO( &fd_socket );
    FD_SET( listen_skt, &fd_socket );

    // 測試用,這邊先不加入write set.
    fd_set fd_read; //, fd_write;
    char recv_buf[1024], send_buf[1024];
    SOCKET max_skt = tcp_get_max_socket(fd_socket);

    struct sockaddr_in remote;
    socklen_t remote_len = sizeof(remote);

    int ret;
    int ret_1, ret_2;
    int count = 0;
    /*
        理論上non-block用write來判斷
        這邊為了方便,直接send回去
    */
    while(1)
    {
        FD_ZERO( &fd_read );
        fd_read = fd_socket;

        //FD_ZERO( &fd_write );
        //fd_write = fd_socket;

        // can set timeout by parameter 5
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        //ret = select( 0, &fd_read, &fd_write, NULL, NULL ); 
        ret = select( max_skt+1, &fd_read, NULL, NULL, &timeout ); 

        if( ret < 0 )
        {
            printf("select fail. ret = %d, break.\n", ret );
            break;
        }
        else if( ret == 0 )        
            printf("time out. ret = %d\n", ret );        

        for( int i = 0; i < (int)fd_socket.fd_count; ++i )
        {
            if ( FD_ISSET( fd_socket.fd_array[i], &fd_read) )
            {
                if( fd_socket.fd_array[i] == listen_skt )
                {
                    memset( &remote, 0, remote_len );
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
                    memset( recv_buf, 0, 1024 );
                    ret_1 = recv( fd_socket.fd_array[i], recv_buf, 1024, 0 );
                    sprintf( send_buf , "Hi. This is server's response. Nice to meet you. count = %d", count++ );                        
                    ret_2 = send( fd_socket.fd_array[i], send_buf , strlen(send_buf), 0 );

                    if( ret_1 == 0 || ret_2 == 0 )
                    {
                        printf("remote disconnected. ret 1 = %d, ret 2 = %d\n", ret_1, ret_2 );
                        closesocket(fd_socket.fd_array[i]);
                        FD_CLR(fd_socket.fd_array[i], &fd_socket);
                    }
                    else if( ret_1 < 0 || ret_2 < 0 )
                    {
                        printf("connect error. ret 1 = %d, ret 2 = %d\n", ret_1, ret_2 );
                        closesocket(fd_socket.fd_array[i]);
                        FD_CLR(fd_socket.fd_array[i], &fd_socket);
                    }
                    else
                    {
                        printf("recv, ret = %d, msg = %s\n", ret_1, recv_buf );
                        printf("send. ret = %d\n", ret_2 );
                    }
                }
            }
        }
    }

    closesocket(listen_skt);
    WSACleanup();
}




