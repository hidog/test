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
#else
typedef int socklen_t;
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



void TcpNb::add_fd_set()
{
    FD_ZERO( &w_set );
    FD_ZERO( &r_set );
    
    FD_SET( listen_skt, &r_set );
    
    for( auto &item : client_list )
    {
        FD_SET( item.skt, &r_set );
        FD_SET( item.skt, &w_set );
    }
}



// 效率不好,但就先不管這個問題了. 畢竟只是範例程式.
SOCKET TcpNb::get_max_skt()
{
    SOCKET max = listen_skt;
    
    for( auto &item : client_list )
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
#else
    int err = WSAGetLastError();
#endif
    return err;
}





void TcpNb::accept_handle()
{
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    memset( &addr, 0, len );

    SOCKET skt = accept( listen_skt, (sockaddr*)&addr, &len );
    printf("accept skt %d. remote %s, %d\n", skt, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port) );

    ClientSocket item;

    item.skt = skt;
    item.connected = true;
    item.task_count = rand() % 10 + 10;
    item.connect_time = time_point_cast<milliseconds>(system_clock::now());

    item.send_state = SendState::UNKNOWN;
    item.recv_state = RecvState::UNKNOWN;

    client_list.push_back(item);
}





void TcpNb::prepare_recv_data( ClientSocket &client )
{
    client.recv_state = RecvState::HEADER;
    client.recv_data.recv_index = 0;   
}




void TcpNb::recv_header( ClientSocket &client )
{
    int shift = client.recv_data.recv_index;
    char *ptr = (char*)&(client.recv_data.header) + shift;
    int remaind = HEADER_SIZE - shift;

    int ret = recv( client.skt, ptr, remaind, 0 );
    printf( "header recv ret = %d\n", ret );

    client.recv_data.recv_index += ret;
    if( client.recv_data.recv_index == HEADER_SIZE )
    {
        printf("recv header. %s %d\n", client.recv_data.header.name, client.recv_data.header.size );
        client.recv_state = RecvState::BODY;   
        client.recv_data.recv_index = 0;
    }
}




void TcpNb::recv_body( ClientSocket &client )
{
    int shift = client.recv_data.recv_index;
    char *ptr = (char*)&(client.recv_data.body.short_data) + shift;
    int remaind = BODY_SIZE_SHORT - shift;

    int ret = recv( client.skt, ptr, remaind, 0 );
    printf("body recv. ret = %d\n", ret );
    client.recv_data.recv_index += ret;

    if( client.recv_data.recv_index == BODY_SIZE_SHORT )
    {
        printf("recv body finish. couont = %d, msg = %s\n", client.recv_data.body.short_data.count, client.recv_data.body.short_data.message );
        client.recv_state = RecvState::STOP;
    }
}




void TcpNb::recv_handle()
{
    //
    if( FD_ISSET( listen_skt, &r_set ) )    
        accept_handle();    

    //
    char buffer[4096];    
    int ret;
    for( auto itr = client_list.begin(); itr != client_list.end(); ++itr )
    {
        if( FD_ISSET( itr->skt, &r_set ) )
        {
            itr->connected = true;
            ClientSocket &client = *itr;

            if( itr->recv_state == RecvState::UNKNOWN )
                prepare_recv_data(client);
            else if( itr->recv_state == RecvState::HEADER )
                recv_header(client);
            else if( itr->recv_state == RecvState::BODY )
                recv_body(client);                        
        }
    }
}




void TcpNb::handle_disconnect( std::vector<ClientSocket>::iterator clt )
{
    printf( "skt %d disconnect.\n", clt->skt );
#ifdef _WIN32
    closesocket(clt->skt);
#else
#error need maintain.
#endif
    client_list.erase(clt);
}



void TcpNb::handle_error( std::vector<ClientSocket>::iterator clt )
{}






int TcpNb::find_client( unsigned long net_ip )
{
    int index;
    for( index = 0; index < client_list.size(); index++ )
    {
        if( client_list[index].net_ip == net_ip )
            break;
    }

    if( index == client_list.size() )
    {
        printf("not found. net_ip = %lu\n", net_ip );
        index = -1;
    }

    return index;
}





void TcpNb::prepare_send_data( ClientSocket &client )
{
    // prepare send header.
    client.send_state = SendState::HEADER;

    // prepare header
    strcpy( client.send_data.header.name, "Slave PC" );
    client.send_data.header.size = PACKET_SHORT_SIZE;
    client.send_data.header.type = PacketType::SHORT;

    // prepare body
    client.send_data.body.short_data.count = 4411;
    sprintf( client.send_data.body.short_data.message, "Hello, this is client" );

    client.send_data.send_index = 0;
    client.send_state = SendState::HEADER;
}





void TcpNb::send_header( ClientSocket &client )
{
    int shift = client.send_data.send_index;
    int remaind = HEADER_SIZE - shift;
    char *ptr = (char*)&client.send_data.header + shift;

    int ret = send( client.skt, ptr, remaind, 0 );
    printf("header send ret = %d.\n", ret );

    client.send_data.send_index += ret;
    if( client.send_data.send_index == HEADER_SIZE )
    {
        printf("send header finish.\n");
        client.send_state = SendState::BODY;
        client.send_data.send_index = 0;
    }
}





void TcpNb::send_body( ClientSocket &client )
{
    int shift = client.send_data.send_index;
    char* ptr = (char*)&client.send_data.body.short_data;
    int remaind = PACKET_SHORT_SIZE - shift;

    int ret = send( client.skt, ptr, remaind, 0 );
    printf("recv body. ret = %d\n", ret );
    client.send_data.send_index += ret;

    if( client.send_data.send_index == PACKET_SHORT_SIZE )
    {
        printf("send body finish.\n" );
        client.send_state = SendState::STOP;
    }   
}







void TcpNb::send_handle()
{
    /*
        隨機產生task
        1. 發送對象
        2. 發送內容. (目前分長訊息跟短訊息)
    */
    
    unsigned long net_ip = inet_addr("127.0.0.1");
    int index = find_client( net_ip );
    if( index == -1 )
        printf("not found. error. ip = %s\n", "127.0.0.1" );
    else
    {
        ClientSocket &client = client_list[index];
        if( FD_ISSET( client.skt, &w_set ) )
        {
            client.connected = true;

            // 放棄效率,選擇一次處理一件事情.
            // 好處是比較好維護. (畢竟是測試用的程式碼)
            if( client.send_state == SendState::UNKNOWN )
                prepare_send_data(client);
            else if( client.send_state == SendState::HEADER )            
                send_header(client);            
            else if( client.send_state == SendState::BODY )
                send_body(client);
        }
    }

}




void TcpNb::connect_to( const char* const ip, int port )
{
    sockaddr_in addr;
    int len = sizeof(addr);
    memset( &addr, 0, len );

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
#ifdef _WIN32
    addr.sin_addr.S_un.S_addr = inet_addr(ip);
#else
#error need maintain.
#endif

    //
    SOCKET skt = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
    set_non_blocking( skt, true );
    if( skt == INVALID_SOCKET )
    {
        int err = get_error_code();
        printf("create socket fail!! error code = %d\n", err);
        return;
    }
    printf("connect to %s %d\n", ip, port );
    int ret = connect( skt, (sockaddr*)&addr, len );
    printf("connect ret = %d\n", ret );
    /*
        non-blocking的情況下有機會立刻連上
        但也可能沒連上
        為了方便統一用select判斷是否連上.
    */

    //
    ClientSocket client;
    client.skt = skt;
    client.connected = false;
    client.connect_time = time_point_cast<milliseconds>(system_clock::now());
    client.net_ip = inet_addr(ip);
    client.task_count = rand() % 10 + 10;

    client.send_state = SendState::UNKNOWN;
    client.recv_state = RecvState::UNKNOWN;

    client_list.push_back(client);
}








int TcpNb::work()
{
    srand( static_cast<unsigned int>(time(NULL)) );

    init();
    setup_server_skt();
    
    bool flag = false;

    //
    int res;
    timeval timeout = { 1, 500000 };
    while(true)
    {
        if( flag == false )
        {
            connect_to("127.0.0.1", port );
            flag = true;
        }

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
            send_handle();
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
    /* 
        If blcok_mode = 0, blocking is enabled; 
        If blcok_mode != 0, non-blocking mode is enabled.
    */
    u_long block_mode = enable ? 0 : 1;
    res = ioctlsocket( skt, FIONBIO, &block_mode );
    if( res != NO_ERROR )
    {
        printf("set non-blocking fail. res = %d\n", res );
        return -1;
    }
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
