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





TcpNb::TcpNb( std::string _pc_name ) 
    : pc_name(_pc_name)
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
    client.recv_state = RecvState::HEAD;
    client.recv_data.recv_index = 0;   
}




void TcpNb::recv_head( ClientSocket &client )
{
    int shift = client.recv_data.recv_index;
    char *ptr = (char*)&(client.recv_data.head) + shift;
    int remaind = HEAD_SIZE - shift;

    int ret = recv( client.skt, ptr, remaind, 0 );
    printf( "recv head ret = %d\n", ret );

    client.recv_data.recv_index += ret;
    if( client.recv_data.recv_index == HEAD_SIZE )
    {
        printf("recv head finish. name = %s, size = %d\n", client.recv_data.head.name, client.recv_data.head.body_size );
        client.recv_state = RecvState::BODY;   
        client.recv_data.recv_index = 0;
    }
}




void TcpNb::recv_body( ClientSocket &client )
{
    int shift = client.recv_data.recv_index;
    PacketType body_type = client.recv_data.head.body_type;

    char *ptr = body_type == PacketType::SHORT ?
                (char*)&(client.recv_data.body.short_data) + shift :
                (char*)&(client.recv_data.body.long_data) + shift;

    int remaind = body_type == PacketType::SHORT ? 
                  SHORT_BODY_SIZE - shift :
                  LONG_BODY_SIZE - shift;

    int finish_size = client.recv_data.head.body_size;
                  
    //
    int ret = recv( client.skt, ptr, remaind, 0 );
    printf("recv body. ret = %d\n", ret );
    client.recv_data.recv_index += ret;

    if( client.recv_data.recv_index == finish_size )
    {
        printf("recv body finish. count = %d, msg = %s\n", client.recv_data.body.long_data.count, client.recv_data.body.long_data.data );
        client.recv_state = RecvState::STOP;
    }
}




void TcpNb::recv_handle()
{
    //
    if( FD_ISSET( listen_skt, &r_set ) )    
        accept_handle();    

    // 
    for( auto itr = client_list.begin(); itr != client_list.end(); ++itr )
    {
        if( FD_ISSET( itr->skt, &r_set ) )
        {
            itr->connected = true;
            ClientSocket &client = *itr;

            if( itr->recv_state == RecvState::UNKNOWN )
                prepare_recv_data(client);
            else if( itr->recv_state == RecvState::HEAD )
                recv_head(client);
            else if( itr->recv_state == RecvState::BODY )
                recv_body(client);                        
        }
    }
}




void TcpNb::handle_disconnect( ClientSocket& client )
{
    printf( "skt %d disconnect. net_ip = %lu\n", client.skt, client.net_ip );

    for( auto itr = client_list.begin(); itr != client_list.end(); ++itr )
    {
        if( itr->net_ip == client.net_ip )
        {
            client_list.erase(itr);
            break;
        }
    }

#ifdef _WIN32
    closesocket(client.skt);
#else
    close(client.skt);
#endif
}





void TcpNb::handle_error( ClientSocket& client )
{
    printf("client error. skt = %d, net_ip = %lu\n", client.skt, client.net_ip );
#ifdef _WIN32
    closesocket(client.skt);
#else
    close(client.skt);
#endif
}






int TcpNb::find_client( unsigned long net_ip )
{
    size_t index;
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
    client.send_data.head.body_type = rand() % 2 == 1 ? PacketType::LONG : PacketType::SHORT;    
    PacketType body_type = client.send_data.head.body_type;

    // prepare send header.
    client.send_state = SendState::HEAD;

    // prepare header
    strcpy( client.send_data.head.name, pc_name.c_str() );
    if( body_type  == PacketType::SHORT )
        client.send_data.head.body_size = SHORT_BODY_SIZE;
    else if( body_type  == PacketType::LONG )
        client.send_data.head.body_size = LONG_BODY_SIZE;
    else
        printf("error\n");    

    // prepare body    
    if( body_type  == PacketType::SHORT )
    {
        client.send_data.body.short_data.count = client.task_count;
        sprintf( client.send_data.body.short_data.message, "Hello, this is client" );
    }
    else if( body_type  == PacketType::LONG )
    {
        client.send_data.body.long_data.count = client.task_count;
        sprintf( client.send_data.body.long_data.data, "Hello, this is cleint" );
    }
    else
        printf("error\n");

    client.send_data.send_index = 0;
    client.send_state = SendState::HEAD;
}





void TcpNb::send_head( ClientSocket &client )
{
    int shift = client.send_data.send_index;
    int remaind = HEAD_SIZE - shift;
    char *ptr = (char*)&client.send_data.head + shift;

    int ret = send( client.skt, ptr, remaind, 0 );
    if( ret < 0 )
        handle_error(client);  
    else if( ret == 0 )
        handle_disconnect(client);
    else
    {
        // send success.
        printf("send head ret = %d.\n", ret );

        client.send_data.send_index += ret;
        if( client.send_data.send_index == HEAD_SIZE )
        {
            printf("send head finish.\n");
            client.send_state = SendState::BODY;
            client.send_data.send_index = 0;
        }
    }
}




void TcpNb::task_finish_handle( ClientSocket &client )
{
    printf("task finish. close socket %d, net ip = %lu\n", client.skt, client.net_ip );
    
    for( auto itr = client_list.begin(); itr != client_list.end(); ++itr )
    {
        if( itr->net_ip == client.net_ip )
        {
            client_list.erase(itr);
            break;
        }
    }

#ifdef _WIN32
    closesocket(client.skt);
#else
    close(client.skt);
#endif
}





void TcpNb::send_body( ClientSocket &client )
{
    int shift = client.send_data.send_index;
    char* ptr = (char*)&client.send_data.body.long_data;
    int remaind = LONG_BODY_SIZE - shift;

    int ret = send( client.skt, ptr, remaind, 0 );
    if( ret < 0 )
        handle_error(client);  
    else if( ret == 0 )
        handle_disconnect(client);
    else
    {
        printf("send body. ret = %d\n", ret );
        client.send_data.send_index += ret;

        if( client.send_data.send_index == LONG_BODY_SIZE )
        {
            printf("send body finish.\n" );
            client.task_count--;
            if( client.task_count == 0 )
                task_finish_handle( client );
            else        
                client.send_state = SendState::UNKNOWN;                
        }   
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
            else if( client.send_state == SendState::HEAD )            
                send_head(client);            
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
