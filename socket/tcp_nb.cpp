#include "tcp_nb.h"

#if defined(MACOS) || defined(UNIX)
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

#include <stdio.h>
#include <string.h>


#if defined(MACOS) || defined(UNIX)
#define SOCKET_ERROR -1
#endif





TcpNb::TcpNb()
{
    ip_list = { 
                "192.168.1.106",  // imac
                "192.168.3.240",  // room
                "192.168.3.191",  // 2F
                "192.168.1.110",  // ubuntu red
                "192.168.1.108",  // ubuntu
                "192.168.2.247",  // x250
                "192.168.2.116",  // mbpr
                "192.168.1.102",  // slave
                "192.168.1.101"   // master
              };
    
}




TcpNb::~TcpNb()
{}



int TcpNb::init()
{
    return 1;
}



void TcpNb::add_fd_set()
{
    FD_ZERO( &w_set );
    FD_ZERO( &r_set );
    
    FD_SET( listen_skt, &r_set );
    
    for( auto item : client_list )
    {
        FD_SET( item.skt, &r_set );
        FD_SET( item.skt, &w_set );
    }
}



// 效率不好,但就先不管這個問題了. 畢竟只是範例程式.
SOCKET TcpNb::get_max_skt()
{
    SOCKET max = listen_skt;
    
    for( auto item : client_list )
    {
        if( item.skt > max )
            max = item.skt;
    }
    
    return max;
}






int TcpNb::get_error_code()
{
#if defined(MACOS) || defined(UNIX)
    int err = errno;
    return err;
#else
#error need maintain.
#endif
}




void TcpNb::recv_handle()
{
    if( FD_ISSET( listen_skt, &r_set ) )
    {
        sockaddr_in addr;
        socklen_t len = sizeof(addr);
        memset( &addr, 0, len );
        
        SOCKET skt = accept( listen_skt, (sockaddr*)&addr, &len );
        printf("accept skt %d. remote %s, %d\n", skt, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port) );
        
        ClientSocket item;
        item.skt = skt;
        item.connected = true;
        item.connect_time = time_point_cast<milliseconds>(system_clock::now());
        
        client_list.push_back(item);
    }
    
    for( auto item : client_list )
    {
        if( FD_ISSET( item.skt, &r_set ) )
        {
        
            
        }
    }
}




int TcpNb::work()
{
    init();
    setup_server_skt();
    
    //
    int res;
    timeval timeout = { 1, 500000 };
    while(true)
    {
        add_fd_set();
        max_skt = get_max_skt();
        
        res = select( max_skt+1, &r_set, &w_set, NULL, &timeout );
        if( res == 0 )
            printf("select timeout.\n");
        else if( res < 0 )
        {
            int err = get_error_code();
            printf("select fail. res = %d, err = %d break\n", res, err );
            break;
        }
        else
        {
            recv_handle();
        }
        
        // select
        // if recv, handle
        // if task, handle
        
        
    }
    
    return 1;
}




int TcpNb::set_non_blocking( SOCKET skt, bool enable )
{
    int res;

#if defined(MACOS) || defined(UNIX)    
    int skt_flags = fcntl( skt, F_GETFL, 0 );
    
    if( enable == true )
        skt_flags |= O_NONBLOCK;
    else
        skt_flags &= (~O_NONBLOCK);
    
    res = fcntl( skt, F_SETFL, skt_flags );
    if( res == -1 )
    {
        printf("set non-block fail.\n");
        return -1;
    }
#else
#error need maintain.
#endif
    
    return 1;
}



int TcpNb::setup_server_skt()
{
    int res;
    
    listen_skt = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
    if( listen_skt == INVALID_SOCKET )
    {
        printf("create listen socket fail.\n");
        return -1;
    }
    set_non_blocking( listen_skt, true );
    
    //
    sockaddr_in addr;
    int addr_len = sizeof(addr);
    memset( &addr, 0, sizeof(addr) );
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    
    res = bind( listen_skt, (sockaddr*)&addr, addr_len );
    if( res == SOCKET_ERROR )
    {
        printf("bind fail\n");
        return -1;
    }
    
    res = listen( listen_skt, SOMAXCONN );
    if( res == SOCKET_ERROR )
    {
        printf("listen fail\n");
        return -1;
    }
    
    return 1;
}
