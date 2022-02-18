// ref : https://www.cnblogs.com/churi/archive/2013/02/27/2935427.html


#ifdef _WIN32
#include <WinSock2.h>
#else
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif


#include <stdio.h>
#include <string.h>

#ifdef _WIN32
// windows可以用這個方式來設定lib
//#pragma comment(lib,"ws2_32.lib") 
#endif


#if defined(UNIX) || defined(MACOS)
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
typedef int SOCKET;
typedef sockaddr* LPSOCKADDR;
#else
typedef int socklen_t;
typedef int ssize_t;
#endif







void tcp_size_server( int port )
{
#ifdef _WIN32
    WORD ws_version = MAKEWORD(2,2);
    WSADATA wsa_data;
    if( WSAStartup(ws_version, &wsa_data) != 0 )
    {
        printf("init error\n");
        return;
    }
#endif

    SOCKET listen_skt = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
    int ret;

    sockaddr_in local_addr;
    socklen_t local_addr_len = sizeof(local_addr);
    memset( &local_addr, 0, local_addr_len );

    local_addr.sin_port = htons(port);
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;

    ret = bind( listen_skt, (sockaddr*)&local_addr, local_addr_len );
    if( ret == SOCKET_ERROR )
    {
        printf("bind fail. ret = %d\n", ret );
        return;
    }

    ret = listen( listen_skt, 1 );
    if( ret == SOCKET_ERROR )
    {
        printf("listen fail. ret = %d\n", ret );
        return;
    }

    printf("listen success.\n");
#ifdef MACOS
    fflush(stdout);
#endif

    //
    sockaddr_in remote_addr;
    socklen_t remote_addr_len = sizeof(remote_addr);
    memset( &remote_addr, 0, remote_addr_len );

    SOCKET skt = accept( listen_skt, (sockaddr*)&remote_addr, &remote_addr_len );
    if( skt == INVALID_SOCKET )
    {
        printf("accept fail.\n");
        return;
    }

    printf("accept. remote = %s, %d\n", inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port) );

    /*
        實驗tcp socket, 傳輸的時候資料不會一次全部傳完
    */
    const int buffer_size = 300;
    char *buf = new char[buffer_size];
    int count = 0;
    while(true)
    {
        for( int i = 0; i < buffer_size; i++ )
            buf[i] = (i%26) + 'a';

        ret = (int)send( skt, buf, buffer_size, 0 );
        if( ret != buffer_size )
        {
            printf("\nsend ret = %d, not %d. break.\n", ret, buffer_size );
            break;
        }
        printf(".");    
#ifdef MACOS
        fflush(stdout);
#endif
        
        if( count % 5 == 0 )
#ifdef _WIN32
            Sleep(10);
#else
            usleep(10000);
#endif
        count++;
    }

    delete [] buf;
    buf = nullptr;

#ifdef _WIN32
    closesocket(skt);
    closesocket(listen_skt);
    WSACleanup();
#else
    close(skt);
    close(listen_skt);
#endif
}









void tcp_hello_server( int port )
{
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
#ifdef _WIN32
        WSACleanup(); 
#endif
        return;
    }

    //
    sockaddr_in local_addr;
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(port);
#ifdef _WIN32
    local_addr.sin_addr.S_un.S_addr = INADDR_ANY; 
#else
    local_addr.sin_addr.s_addr = INADDR_ANY;
#endif

    // note: linux not support LPSOCKADDR, need defined it.
    int res = bind( listen_skt, (LPSOCKADDR)&local_addr, sizeof(local_addr) );  // LPSOCKADDR 在window下可以取代 sockaddr*
    if( res == SOCKET_ERROR)   
    {
#ifdef _WIN32
        int err = WSAGetLastError();
#else
        int err = errno;
#endif
        printf( "bind error! res = %d, err = %d\n", res, err );
#ifdef _WIN32
        closesocket(listen_skt);
        WSACleanup();
#else
        close(listen_skt);
#endif
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
#ifdef _WIN32
        closesocket(listen_skt);
        WSACleanup();
#else
        close(listen_skt);
#endif
        return;
    }

    //
    SOCKET client_skt;
    sockaddr_in remote_addr;
    socklen_t remote_addr_len = sizeof(remote_addr);
    char recv_buf[255]; 
    char send_buf[255];

    //
    client_skt = accept( listen_skt, (sockaddr*)&remote_addr, &remote_addr_len );
    if( client_skt == INVALID_SOCKET )
    {
#ifdef _WIN32
        int err = WSAGetLastError();
#else
        int err = errno;
#endif
        printf( "accept error ! client_skt = %d, err = %d\n", (int)client_skt, err );
#ifdef _WIN32
        closesocket(listen_skt);
        WSACleanup();
#else
        close(listen_skt);
#endif
        return;
    }   
    printf("accept from : %s, port = %d", inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port) );

    // recv
    memset( recv_buf, 0, 255 );
    ssize_t ret = recv( client_skt, recv_buf, 255, 0 );        
    if( ret > 0 )
    {
        printf( "recv. ret = %d\n", (int)ret );
        printf( "msg = %s\n", recv_buf );
    }
    else if( ret <= 0 )
    {
#ifdef _WIN32
        int err_code = WSAGetLastError();  
#else
        int err_code = errno;
#endif
        printf("recv ret = %d, err = %d, end.\n", (int)ret, err_code );
#ifdef _WIN32
        closesocket(listen_skt);
        closesocket(client_skt);
        WSACleanup();
#else
        close(listen_skt);
        close(client_skt);
#endif
        return;
    }

    // send
    memset( send_buf, 0, 255 );
    sprintf( send_buf, "hello, this is server." );
    ret = send( client_skt, send_buf, (int)strlen(send_buf), 0 );    
    if( ret > 0 )
        printf("send. ret = %d\n", (int)ret );
    else
    {
#ifdef _WIN32
        int err = WSAGetLastError();
#else
        int err = errno;
#endif        
        printf("send fail. ret = %d, err = %d\n", (int)ret, err );
#ifdef _WIN32
        closesocket(listen_skt);
        closesocket(client_skt);
        WSACleanup();
#else
        close(listen_skt);
        close(client_skt);
#endif
        return;
    }   
    
    // 
#ifdef _WIN32
    closesocket(client_skt);
    closesocket(listen_skt);
    WSACleanup();
#else
    close(client_skt);
    close(listen_skt);
#endif

    return;
}





void tcp_size_client( const char* const ip, int port )
{
#ifdef _WIN32
    WORD ws_version = MAKEWORD(2,2);
    WSADATA wsa_data;
    if( WSAStartup( ws_version, &wsa_data) != 0 )
    {
        printf("init error\n");
        return;
    }
#endif

    SOCKET skt = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
    if( skt == INVALID_SOCKET )
    {
        printf("get skt fail.\n");
        return;
    }

    //
    sockaddr_in remote_addr;
    socklen_t remote_addr_len = sizeof(remote_addr);

    memset( &remote_addr, 0, remote_addr_len );
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(port);
    remote_addr.sin_addr.s_addr = inet_addr(ip);

    int ret;
    ret = connect( skt, (sockaddr*)&remote_addr, remote_addr_len );
    if( ret == SOCKET_ERROR )
    {
        printf("connect fail. ret = %d\n", ret );
        return;
    }

    const int buffer_size = 300;
    char *buf = new char[buffer_size];
    int count = 0;

    while(true)
    {
        memset( buf, 0, buffer_size );

        ret = (int)recv( skt, buf, buffer_size, 0 );
        if( ret != buffer_size )
        {
            printf( "\nret = %d, size = %d, break\n", ret, buffer_size );
            break;
        }
        
        printf( "%c", buf[count] );
        count = (count+1) % buffer_size;
    }

#ifdef _WIN32
    closesocket(skt);
    WSACleanup();
#else
    close(skt);
#endif
}







void tcp_hello_client( const char *ip, int  port )
{
#ifdef _WIN32
    WORD sock_version = MAKEWORD(2,2);
    WSADATA wsa_data; 
    if( 0 != WSAStartup(sock_version, &wsa_data) )
    {
        printf("init error\n");
        return;
    }
#endif

    SOCKET skt = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
    if( skt == INVALID_SOCKET )
    {
        printf( "invalid socket! skt = %d\n", (int)skt );
#ifdef _WIN32
        WSACleanup();
#endif
        return;
    }

    sockaddr_in remote_addr;
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(port);
#ifdef _WIN32
    remote_addr.sin_addr.S_un.S_addr = inet_addr(ip); 
#else
    remote_addr.sin_addr.s_addr = inet_addr(ip);
#endif

    //
    int res = connect( skt, (sockaddr *)&remote_addr, sizeof(remote_addr) );
    if( res == SOCKET_ERROR )
    {
#ifdef _WIN32
        int err = WSAGetLastError();
#else
        int err = errno;
#endif
        printf("connect error! res = %d, err = %d\n", res, err );
#ifdef _WIN32
        closesocket(skt);
        WSACleanup();
#else
        close(skt);
#endif
        return;
    }

    //
    const char *send_buf = "hello, server. this is client.";
    ssize_t ret = send( skt, send_buf, (int)strlen(send_buf), 0 );
    if( ret > 0 )    
        printf( "send. ret = %d\n", (int)ret );        
    else
    {
#ifdef _WIN32
        int err = WSAGetLastError();
#else
        int err = errno;
#endif
        printf( "send fail. ret = %d, err = %d\n", (int)ret, err );
#ifdef _WIN32
        closesocket(skt);
        WSACleanup();
#else
        close(skt);
#endif
        return;
    }

    //
    char recv_buf[255] = {0};
    ret = recv( skt, recv_buf, 255, 0 );
    if( ret > 0 )    
        printf( "recv. ret = %d, msg = %s\n", (int)ret, recv_buf );
    else
    {
#ifdef _WIN32
        int err = WSAGetLastError();
#else
        int err = errno;
#endif
        printf( "recv fail. ret = %d, err = %d\n", (int)ret, err );
#ifdef _WIN32
        closesocket(skt);
        WSACleanup();
#else
        close(skt);
#endif
        return;
    }
    
#ifdef _WIN32
    closesocket(skt);
    WSACleanup();
#else
    close(skt);
#endif

    return ;
}







void tcp_client_timeout_test()
{
#ifdef _WIN32
    WORD sock_version = MAKEWORD(2,2);
    WSADATA wsa_data; 
    if( 0 != WSAStartup(sock_version, &wsa_data) )
    {
        printf("init error\n");
        return;
    }
#endif

    SOCKET skt = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
    if( skt == INVALID_SOCKET )
    {
        printf( "invalid socket! skt = %d\n", (int)skt );
        return;
    }

    sockaddr_in remote_addr;
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(3425);
    remote_addr.sin_addr.s_addr = inet_addr("192.168.1.142");

    // 有網頁說無法設置connect timeout,但實際測試有成功
    // note: windows底下測試, connect timeout沒效果.
#if defined(UNIX) || defined(MACOS)
    timeval timeout;
    socklen_t timeout_len = sizeof(timeout);
#else
    int timeout;
    int timeout_len = sizeof(timeout);
#endif
    getsockopt( skt, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, &timeout_len );

#ifdef _WIN32
    printf("timeout = %d\n", timeout );
#elif defined(UNIX)
    printf("timeout = %ld, %ld\n", timeout.tv_sec, timeout.tv_usec );
#elif defined(MACOS)
    printf("timeout = %ld, %d\n", timeout.tv_sec, timeout.tv_usec );
#endif

#ifdef _WIN32
    timeout = 1;
#else
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
#endif
    // mac環境測試結果, timeout沒成功
    setsockopt( skt, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout) );

    //
    printf("start connect...\n");
    int res = connect( skt, (sockaddr *)&remote_addr, sizeof(remote_addr) );
    if( res == SOCKET_ERROR )
    {
#ifdef _WIN32
        int err = WSAGetLastError();
#else
        int err = errno;
#endif
        printf("connect error! res = %d, err = %d\n", res, err );
        return;
    }
    printf("connected.\n");

    // 本來懷疑沒設回來會造成timeout過短,但實測並沒有
    // 就算直接close skt也沒有觸發timeout,需要研究原因
    // 試了很多方式, send都成功, 沒有觸發timeout.
    // 實務上blocking的寫法不太需要設timeout,就先不追究了.
    // timeout.tv_sec = 0;
    // timeout.tv_usec = 0;
    // setsockopt( skt, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout) );

    //
    const char *send_buf = "hello, server. this is client.";
    ssize_t ret = send( skt, send_buf, (int)strlen(send_buf), 0 );
    if( ret > 0 )    
        printf( "send. ret = %d\n", (int)ret );        
    else if( ret == 0 )
        printf("remote disconnect\n");
    else
    {
#ifdef _WIN32
        int err = WSAGetLastError();
#else
        int err = errno;
#endif
        printf( "send fail. ret = %d, err = %d\n", (int)ret, err );
        return;
    }
    
#ifdef _WIN32
    closesocket(skt);
    WSACleanup();
#else
    close(skt);
#endif

    return;
}






void tcp_server_timeout_test()
{
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
    sockaddr_in local_addr;
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(3425);
    local_addr.sin_addr.s_addr = INADDR_ANY;

    // note: linux not support LPSOCKADDR, need defined it.
    int res = bind( listen_skt, (LPSOCKADDR)&local_addr, sizeof(local_addr) );  // LPSOCKADDR 在window下可以取代 sockaddr*
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
    sockaddr_in remote_addr;
    socklen_t remote_addr_len = sizeof(remote_addr);
    char recv_buf[255]; 

    printf("listened. wait for accept...\n");

    // ubuntu下設定timeout有作用
    // 沒設定的話accept似乎沒有timeout.
    // windows測試沒成功
#ifdef _WIN32
    int timeout = 1;
#else
    timeval timeout = { 1, 0 };
#endif
    setsockopt( listen_skt, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout) );

    //
    client_skt = accept( listen_skt, (sockaddr*)&remote_addr, &remote_addr_len );
    if( client_skt == INVALID_SOCKET )
    {
#ifdef _WIN32
        int err = WSAGetLastError();
#else
        int err = errno;
#endif
        printf( "accept error ! client_skt = %d, err = %d\n", (int)client_skt, err );
        return;
    }   
    printf("accept from : %s, port = %d\n", inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port) );
#ifdef _WIN32
    closesocket(client_skt);
#else
    close(client_skt);
#endif

    // recv
    memset( recv_buf, 0, 255 );
    ssize_t ret = recv( client_skt, recv_buf, 255, 0 );        
    if( ret > 0 )
    {
        printf( "recv. ret = %d\n", (int)ret );
        printf( "msg = %s\n", recv_buf );
    }
    else if( ret <= 0 )
    {
#ifdef _WIN32
        int err_code = WSAGetLastError();  
#else
        int err_code = errno;
#endif
        printf("recv ret = %d, err = %d, end.\n", (int)ret, err_code );
        return;
    }

    // 
#ifdef _WIN32
    closesocket(client_skt);
    closesocket(listen_skt);
    WSACleanup();
#else
    close(client_skt);
    close(listen_skt);
#endif

    return;
}






