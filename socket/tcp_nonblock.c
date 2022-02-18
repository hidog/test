#include "tcp_nonblock.h"

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <WinSock2.h>
#else
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#endif



#ifdef _WIN32
typedef int socklen_t;
#else
typedef struct sockaddr* PSOCKADDR;
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#endif




// 測試用,直接寫死用array. 用來管理socket. 不是好寫法,方便管理而已
#define MAX_SOCKET_SIZE 10
SOCKET socket_array[MAX_SOCKET_SIZE] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }; 
int socket_size = 0;
fd_set server_set;




void insert_socket( SOCKET skt )
{
    if( socket_size == MAX_SOCKET_SIZE )
    {
        printf("socket_array full. error!!\n");
        return;
    }
    
    int i = 0;
    for( ; i < socket_size; i++ )
    {
        if( socket_array[i] == -1 )
            break;
    }
    socket_array[i] = skt;
    socket_size++;
}




void tcp_show_ip_address( SOCKET skt )
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    
    // 這邊沒處理error code. 一般來講需要根據回傳值做判斷
    getsockname( skt, (struct sockaddr*)&addr, &len );
    printf("current ip address %s, %d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port) );

    getpeername( skt, (struct sockaddr*)&addr, &len );
    printf("remote ip address %s, %d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port) );

}





void remove_socket( SOCKET skt )
{
    int i;
    for( i = 0; i < socket_size; i++ )
    {
        if( socket_array[i] == skt )
            break;
    }
        
    int j;
    for( j = MAX_SOCKET_SIZE-1; j >= 0; j-- )
    {
        if( socket_array[j] != -1 )
            break;
    }
    
    if( j >= 0 )
    {
        socket_array[i] = socket_array[j];
        socket_array[j] = -1;
    }
}







void tcp_client_connect_test_1( SOCKET skt, SOCKADDR_IN addr )
{
    int res;
    res = connect( skt, (PSOCKADDR)&addr, sizeof(addr) );

    if( res == 0 )
        printf("connect !!\n");
    else
    {
        while(1)
        {
            int error = 0;
            socklen_t len = sizeof (error);
            int retval = getsockopt (skt, SOL_SOCKET, SO_ERROR, (char*)&error, &len);
            printf("error = %d, retval = %d\n", error, retval );
#ifdef _WIN32
            if( error == WSAEISCONN )
#elif defined(UNIX) || defined(MACOS)
            if( error == EISCONN )
#endif
            {
                printf("conencted\n");
                break;
            }
#ifdef _WIN32
            Sleep(1000);
#else
            sleep(1);
#endif
        }
    }
}




void tcp_client_connect_test_2( SOCKET skt, SOCKADDR_IN addr )
{
    int res;

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
#ifdef _WIN32
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
#elif defined(UNIX) || defined(MACOS)
            int err = errno;
            if( err == EINPROGRESS )
                printf("connecting...\n");
            else if( err == EISCONN )
            {
                printf("connected!!\n");
                break;
            }
            else
            {
                printf("connect fail. err = %d\n", err);
                return;
            }
#endif
        }        
    }
}






void tcp_client_connect_test_3( SOCKET skt, SOCKADDR_IN addr )
{
    int res;
    fd_set r_set, w_set, e_set;
    struct timeval timeout = { 1, 0 };

    res = connect( skt, (PSOCKADDR)&addr, sizeof(addr) );
    if( res == 0 )    
        printf("connect success. res = %d\n", res );    
    else
    {
        int timeout_count;
        // 測試結果只能用timeout, 如果一直跑迴圈, 並沒有得到錯誤訊息.
        // 只有測試windows
        for( timeout_count = 0; timeout_count < 10000; timeout_count++ ) 
        { 
            FD_ZERO( &r_set );
            FD_ZERO( &w_set );
            FD_ZERO( &e_set );

            FD_SET( skt, &r_set );
            FD_SET( skt, &w_set );
            FD_SET( skt, &e_set );

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
                printf("res = %d\n", res);
                
#if defined(UNIX) || defined(MACOS) 
                // 假設對象電腦開機,這邊會回傳 113 的 error code.
                // 關機的話不會跳錯誤
                // 行為跟windows不同
                // #define EHOSTUNREACH 113 /* No route to host
                // mac測的結果,在73次的時候跳錯. (沒連線成功但顯示connected
                // mac的error code = 60, Device not a stream
                // 查了一下,發生error的socket也會被標記為可讀寫.
                int error = 0;
                socklen_t len = sizeof (error);
                int retval = getsockopt (skt, SOL_SOCKET, SO_ERROR, (char*)&error, &len);               
                printf("error = %d. retval = %d\n", error, retval );
#endif
                
                // 用r_set會fail.
                if( FD_ISSET( skt, &w_set ) )
                {
                    printf("connected!!\n");
                    break;
                }
                if( FD_ISSET( skt, &e_set ) ) // 實際上沒抓到timeout訊息
                {
                    int error = 0;
                    socklen_t len = sizeof (error);
                    int retval = getsockopt (skt, SOL_SOCKET, SO_ERROR, (char*)&error, &len);
                    printf("error = %d. retval = %d\n", error, retval );
#ifdef _WIN32
                    if( error == WSAEISCONN )
#elif defined(UNIX) || defined(MACOS)
                    if( error == EISCONN )
#endif
                    {
                        printf("conencted\n");
                        break;
                    }
                }
            }
        } 

        if( timeout_count == 10 )
        {
            printf("connected fail.\n");
            return;
        }
    }
}





void tcp_client_connect_test_4( SOCKET skt, SOCKADDR_IN addr )
{
    int res;
    fd_set r_set, w_set;
    struct timeval timeout = { 1, 0 };

    // 底下這段code可以運作
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
#ifdef _WIN32
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
#else
                    int err = errno;
                    if( err == EINPROGRESS )
                        printf("in progress...\n");                        
                    else if( err == EISCONN )
                    {
                        printf("connected!\n");
                        break;
                    }
                    else
                    {
                        printf("err = %d. connect fail.\n", err );
                        return;
                    }
#endif
                }
            }
        } 

        if( timeout_count == 10 )
            printf("timeout. connect fail\n");
    }
}







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


    /*
        non-block的狀態下, 呼叫connect高機率返回 error, 
        需要用select判斷什麼時候連上線
    */
#if 0
    // 底下這段code測試失敗,不能這樣連線
    // 用 getsockopt 抓 error code,沒成功.
    tcp_client_connect_test_1( skt, addr );

#elif 0
    // 這段code得到 WSAEWOULDBLOCK 的error code
    // 因為non-block模式下connect會立刻回傳.
    tcp_client_connect_test_2( skt, addr );

#elif 0
    // 這段code可以運作, 但本身有問題, 比較喜歡底下的做法 
    // 用 getsockopt 判斷連線成功
    tcp_client_connect_test_3( skt, addr );

#else
    //  這段code也有成功    
    // 會不停 connect
    tcp_client_connect_test_4( skt, addr );
#endif

    char buffer[1024];
    int count = 0;
    int ret;

    fd_set r_set, w_set;
    struct timeval timeout;

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
        ret = (int)send( skt, buffer, strlen(buffer), 0 );
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
                    ret = (int)recv( skt, buffer, 1024, 0 );
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

#ifdef _WIN32
        Sleep(1000);
#else
        sleep(1);
#endif
    }


    tcp_close_socket(skt);
#ifdef _WIN32
    WSACleanup();
#endif
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

    //while(1)
    for( int i = 0; i < 10; i++ )
    {
        sprintf( buffer, "Hi, this is client. non-blocking tcp test. count = %d.", count++ );
        ret = (int)send( skt, buffer, strlen(buffer), 0 );
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
        ret = (int)recv( skt, buffer, 1024, 0 );
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
#ifdef _WIN32
        Sleep(1000);
#else
        sleep(1);
#endif
    }

    tcp_close_socket(skt);
#ifdef _WIN32
    WSACleanup();
#endif
}




SOCKADDR_IN tcp_setup_addr_client( const char* const ip, int port )
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
    return 1;
}



int tcp_set_nonblocking( SOCKET skt )
{
    int res;

#ifdef _WIN32
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
#else
    int skt_flags = fcntl( skt, F_GETFL, 0 );
    skt_flags |= O_NONBLOCK;
    res = fcntl( skt, F_SETFL, skt_flags );
    if( res == -1 )
    {
        printf("set non-block fail.\n");
        return -1;
    }
#endif
    
    return 1;
}







void tcp_close_socket( SOCKET skt )
{
#ifdef _WIN32
    closesocket(skt);
#else
    close(skt);
#endif
}








/* 
    看起來windows可以直接存取fd_set,但ubuntu不行
    最後用array簡單處理
*/
SOCKET tcp_get_max_socket( void )
{
    SOCKET max = 0;
    for( int i = 0; i < socket_size; i++ )
    {
        //if( max < FDSET_ARRAY(set,i) )
          //  max = FDSET_ARRAY(set,i);
        if( max < socket_array[i] )
            max = socket_array[i];
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
    
    return 1;
}





SOCKET tcp_server_accept( SOCKET listen_skt )
{
    int res;
    struct sockaddr_in remote;
    socklen_t remote_len = sizeof(remote);

    memset( &remote, 0, remote_len );
    SOCKET skt = accept( listen_skt, (struct sockaddr*)&remote, &remote_len );
    res = tcp_set_nonblocking(skt);
    if( res < 0 )
    {
        printf("set non-blocking fail. skt = %d.\n", res );
        return INVALID_SOCKET;
    }
    FD_SET( skt, &server_set );
    insert_socket(skt);
    printf("a new client connected! from ip = %s, port = %d...\n", inet_ntoa(remote.sin_addr), ntohs(remote.sin_port) );

    tcp_show_ip_address(skt);

    return skt;
}






void tcp_server_rs( SOCKET skt )
{
    static int count = 0;
    char recv_buf[1024], send_buf[1024];
    int ret_1, ret_2;

    memset( recv_buf, 0, 1024 );
    ret_1 = (int)recv( skt, recv_buf, 1024, 0 );
    sprintf( send_buf , "Hi. This is server's response. Nice to meet you. count = %d", count++ );                        
    ret_2 = (int)send( skt, send_buf , strlen(send_buf), 0 );

    if( ret_1 == 0 || ret_2 == 0 )
    {
        printf("remote disconnected. ret 1 = %d, ret 2 = %d\n", ret_1, ret_2 );
        tcp_close_socket( skt );
        FD_CLR( skt, &server_set );
        remove_socket( skt );
    }
    else if( ret_1 < 0 || ret_2 < 0 )
    {
        printf("connect error. ret 1 = %d, ret 2 = %d\n", ret_1, ret_2 );
        tcp_close_socket( skt );
        FD_CLR( skt, &server_set );
        remove_socket( skt );
    }
    else
    {
        printf("recv, ret = %d, msg = %s\n", ret_1, recv_buf );
        printf("send. ret = %d\n", ret_2 );
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
    insert_socket(listen_skt);

    res = tcp_set_nonblocking(listen_skt);
    if( res < 0 )
        return;

    //
    res = tcp_setup_listen_skt( listen_skt, port );
    if( res < 0 )
        return;

    FD_ZERO( &server_set );
    FD_SET( listen_skt, &server_set );

    // 測試用,這邊先不加入write set.
    fd_set fd_read; //, fd_write;
    fd_set fd_excep;
    SOCKET max_skt = tcp_get_max_socket();

    int ret;
    /*
        理論上non-block用write來判斷
        這邊為了方便,直接send回去
    */
    while(1)
    {
        max_skt = tcp_get_max_socket();
        
        FD_ZERO( &fd_read );
        FD_ZERO( &fd_excep );
        fd_read = server_set;
        fd_excep = server_set;

        //FD_ZERO( &fd_write );
        //fd_write = fd_socket;

        // can set timeout by parameter 5
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        //ret = select( 0, &fd_read, &fd_write, NULL, NULL ); 
        ret = select( max_skt+1, &fd_read, NULL, &fd_excep, &timeout ); 

        if( ret < 0 )
        {
            printf("select fail. ret = %d, break.\n", ret );
            break;
        }
        else if( ret == 0 )        
            printf("time out. ret = %d\n", ret );

        for( int i = 0; i < socket_size; i++ )
        {
            // 測試結果,最後是底下抓到斷線訊息. 斷線跟exception set無關.
            if ( FD_ISSET( socket_array[i], &fd_excep ) )             
                printf("get fd except\n");            
            else if ( FD_ISSET( socket_array[i], &fd_read) )
            {            
                if( socket_array[i] == listen_skt )                
                    tcp_server_accept( listen_skt );                
                else
                    tcp_server_rs( socket_array[i] );
            }
        }
    }

    tcp_close_socket(listen_skt);
#ifdef _WIN32
    WSACleanup();
#endif
}




