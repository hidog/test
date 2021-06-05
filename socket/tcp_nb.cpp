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




TcpNb::TcpNb( std::string _pc_name, int _port ) 
    : pc_name(_pc_name), port(_port)
{
    ip_list = { 
                "36.231.106.136",   // imac
                "36.231.106.136",   // room
                "36.231.97.222",    // ubuntu red
                "111.248.196.108",  // slave
                "122.116.84.59"     // master
              };

    port_list = { 
                    1234,  // imac
                    1235,  // room
                    1240,  // ubuntu red
                    1234,  // slave
                    1234   // master
                };

    //ip_list = { "127.0.0.1" };
    //port_list = { 1234 };

    task.finished = false;
    task.timestamp = time_point_cast<milliseconds>(system_clock::now());
    task.ip = ip_list[0];
    task.port = port_list[0];
    task.type = PacketType::SHORT;
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
    if( skt == SOCKET_ERROR )
    {

/*#ifdef UNIX
        // unix平台,強制中斷程式,第二次執行程式的時候會遇到這個錯誤.
        // 需要將listen socket改成reuse.
        int err = errno;
        printf("accept fail. err = %d. need re-setupt listen socket.\n", err);
        close(listen_skt);
        setup_server_skt();
#else*/
        printf("accept fail.\n");
//#endif
        return;
    }
    
    printf("accept skt %d. remote %s, %d\n", skt, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port) );

    ClientSocket item;

    item.port = ntohs(addr.sin_port);
    item.net_ip = addr.sin_addr.s_addr;

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

    int ret = (int)recv( client.skt, ptr, remaind, 0 );
    if( ret < 0 )
        handle_error(client);
    else if( ret == 0 )
        handle_disconnect(client);
    else
    {
        // recv success.
        printf( "recv head ret = %d\n", ret );

        client.recv_data.recv_index += ret;
        if( client.recv_data.recv_index == HEAD_SIZE )
        {
            printf("recv head finish. name = %s, size = %d\n", client.recv_data.head.name, client.recv_data.head.body_size );
            client.recv_state = RecvState::BODY;   
            client.recv_data.recv_index = 0;
        }
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
                  
    //
    int ret = (int)recv( client.skt, ptr, remaind, 0 );
    if( ret < 0 )
        handle_error(client);
    else if( ret == 0 )
        handle_disconnect(client);
    else
    {
        // recv success.
        printf("recv body. ret = %d\n", ret );
        client.recv_data.recv_index += ret;
        remaind -= ret;

        if( remaind == 0 )
        {
            printf("recv body finish. count = %d, msg = %s\n", client.recv_data.body.long_data.count, client.recv_data.body.long_data.data );
            client.task_count--;
            client.recv_state = RecvState::UNKNOWN;
        }
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
        if( itr->connected == true && FD_ISSET( itr->skt, &r_set ) )
        {
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
    printf( "skt %d disconnect. net_ip = %u\n", client.skt, client.net_ip );
    for( auto itr = client_list.begin(); itr != client_list.end(); ++itr )
    {
        if( itr->net_ip == client.net_ip && itr->port == client.port )
        {
            itr->task_count = 0;
            break;
        }
    }
}





void TcpNb::handle_error( ClientSocket& client )
{
    printf("client error. skt = %d, net_ip = %u\n", client.skt, client.net_ip );
    client.task_count = 0;
}






int TcpNb::find_client( unsigned long net_ip, int port )
{
    size_t index;
    for( index = 0; index < client_list.size(); index++ )
    {
        if( client_list[index].net_ip == net_ip && client_list[index].port == port )
            break;
    }

    if( index == client_list.size() )
    {
        printf("not found. net_ip = %lu\n", net_ip );
        index = -1;
    }

    return static_cast<int>(index);
}





void TcpNb::prepare_send_data( ClientSocket &client )
{
    client.send_data.head.body_type = task.type;    
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

    int ret = (int)send( client.skt, ptr, remaind, 0 );
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




void TcpNb::task_finish_handle()
{
    /*
        實際上需要做標記 task finish, 並在work最後面一起把資料清掉.
        這邊就不做這件事情了, 缺點是有機會count為負數.
    */

    std::vector<ClientSocket>::iterator itr = client_list.begin();
    while(true)
    {
        // 一定要先檢查end, 不然會crash.
        if( itr == client_list.end() )
            break;
        else if( itr->task_count <= 0 )
        {
            printf("task finish. close socket %d, net ip = %u, port = %d\n", itr->skt, itr->net_ip, itr->port );      
#ifdef _WIN32
            closesocket(itr->skt);
#else
            close(itr->skt);
#endif
            auto old_itr = itr;
            itr = client_list.erase(old_itr);
        }
        else
            ++itr;
    }
}





void TcpNb::send_body( ClientSocket &client )
{
    int shift = client.send_data.send_index;
    char* ptr = (char*)&client.send_data.body.long_data;

    int remaind = client.send_data.head.body_type == PacketType::LONG ? 
                  LONG_BODY_SIZE - shift :
                  SHORT_BODY_SIZE - shift;

    int ret = (int)send( client.skt, ptr, remaind, 0 );
    if( ret < 0 )
        handle_error(client);  
    else if( ret == 0 )
        handle_disconnect(client);
    else
    {
        printf("send body. ret = %d\n", ret );
        client.send_data.send_index += ret;
        remaind -= ret;

        if( remaind == 0 )
        {
            printf("send body finish.\n" );
            client.task_count--;      
            client.send_state = SendState::UNKNOWN;                
            task.finished = true;
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
    if( task.finished == true )
        return;

    unsigned long net_ip = inet_addr(task.ip.c_str());
    int index = find_client( net_ip, task.port );
    
    if( index == -1 )
        printf("send not found. error. ip = %s\n", task.ip.c_str() );
    else
    {   
        ClientSocket &client = client_list[index];
        if( client.connected == true && FD_ISSET( client.skt, &w_set ) )
        {        
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
    addr.sin_addr.s_addr = inet_addr(ip);
#endif

    //
    SOCKET skt = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
    set_blocking( skt, false );
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
    client.port = port;
    client.task_count = rand() % 10 + 10;

    client.send_state = SendState::UNKNOWN;
    client.recv_state = RecvState::UNKNOWN;

    client_list.push_back(client);
}




void TcpNb::handle_connect_timeout()
{
    for( auto itr = client_list.begin(); itr != client_list.end(); ++itr )
    {
        if( itr->connected == false )
        {
            auto now = time_point_cast<milliseconds>(system_clock::now());
            auto diff = duration_cast<milliseconds>( now - itr->connect_time ).count();
            if( diff > 10000 )
            {
                // timeout = 10s            
                itr->task_count = 0; // 用mark的方式移除 
                printf("skt %d, net ip %u, connect timeout.\n", itr->skt, itr->net_ip );
            }
        }
    }
}





void TcpNb::get_new_task()
{
    // 每秒一個task
    time_point<system_clock,milliseconds> now = time_point_cast<milliseconds>(system_clock::now());
    long long diff = duration_cast<milliseconds>( now - task.timestamp ).count();
    if( diff <= 1000 )
        return;    

    int index;

    task.finished = false;
    index = rand() % ip_list.size();
    task.ip = ip_list[index];
    task.port = port_list[index];
    task.type = rand()%2 == 1 ? PacketType::LONG : PacketType::SHORT;
    task.timestamp = time_point_cast<milliseconds>(system_clock::now());

    printf("create task. ip = %s, port = %d\n", task.ip.c_str(), task.port );
}






bool TcpNb::is_exist_client( std::string ip, int port )
{
    bool res = false;
    unsigned long net_ip = inet_addr(ip.c_str());
    for( auto &item : client_list )
    {
        if( item.net_ip == net_ip && item.port == port )
        {
            res = true;
            break;
        }
    }
    return res;
}




/*
    測試結果用這個方式是目前比較好的一個解法
    直接用select  send/recv判斷是否連上線,會出問題
    在UNIX環境下,對方電腦開著,有機會select通過
    在send/recv的時候得到error code EHOSTUNREACH 113, No route to host 
    最後選擇用connect做判斷
*/
void TcpNb::connect_handle()
{
    //printf("connect_handle 1\n");
    for( auto itr = client_list.begin(); itr != client_list.end(); ++itr )
    {
        // 測了一下用w_set可以判斷是否連上線
        //if( itr->connected == false && ( FD_ISSET(itr->skt, &r_set) || FD_ISSET(itr->skt, &w_set) ) )
        if( itr->connected == false && FD_ISSET(itr->skt, &w_set)  )
        {
            // 實際上可以暫存addr來加快處理速度
            sockaddr_in addr;
            
            addr.sin_family = AF_INET;
            addr.sin_port = htons(itr->port);
            addr.sin_addr.s_addr = itr->net_ip;
            
            int res = connect( itr->skt, (sockaddr*)&addr, sizeof(addr) );
            //printf("connect handle res = %d\n", res);
            if( res == 0 )
            {
                printf("connected! res = %d\n", res );
                itr->connected = true;
            }
            else if( res == SOCKET_ERROR )
            {
#ifdef _WIN32
                int error = WSAGetLastError();  // win32用底下的code執行沒成功,會失敗.
#else
                int error = errno;
                // 底下的code似乎只能在ubuntu底下work. 在windows, macos都會取得error = 0
                //socklen_t len = sizeof(error);
                //getsockopt( itr->skt, SOL_SOCKET, SO_ERROR, (char*)&error, &len );
                //printf("error = %d\n", error );         
#endif
                
#ifdef _WIN32
                if( error == WSAECONNREFUSED )
#else
                if( error == ECONNREFUSED )
#endif
                {
                    itr->task_count = 0;
                    printf("connect fail. CONNREFUSED\n" ); 
                }                
#ifdef _WIN32
                else if( error == WSAEISCONN )  
#else
                else if( error == EISCONN )
#endif
                {
                    itr->connected = true;
                    printf("connected. skt = %d\n", itr->skt );
                }
#ifdef _WIN32
                if( error == WSAEINPROGRESS )
#else
                if( error == EINPROGRESS )
#endif
                {
                    printf("in progress...\n");
                }

            }
        }
    }
}






int TcpNb::work()
{
    srand( static_cast<unsigned int>(time(NULL)) );

    init();
    setup_server_skt();
   
    //
    int res;
    timeval timeout = { 1, 500000 };
    while(true)
    {
        if( task.finished == true )
            get_new_task();

        if( is_exist_client(task.ip, task.port) == false )
            connect_to( task.ip.c_str(), task.port );

        add_fd_set();
        max_skt = get_max_skt();
        
        res = select( max_skt+1, &r_set, &w_set, NULL, &timeout );
        //printf("select res = %d\n", res);
        if( res == 0 )
        {
            //printf("select timeout.\n");                
        }
        else if( res < 0 )
        {
            int err = get_error_code();
            printf("select fail. res = %d, err = %d break\n", res, err );
            break;
        }
        else
        {
            connect_handle();
            recv_handle();
            send_handle();
        }
        handle_connect_timeout();        
        task_finish_handle();
    }
    
    return 1;
}




int TcpNb::set_blocking( SOCKET skt, bool enable )
{
    int res;

#if defined(MACOS) || defined(UNIX)    
    int skt_flags = fcntl( skt, F_GETFL, 0 );
    
    if( enable == false )
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
    
#if defined(UNIX) || defined(MACOS)
    // ubuntu環境,如果沒設置這個選項,強制中斷程式後,會造成無法連線,
    // 需要close listen socket後重新bind, listen.
    int optval = 1;
    setsockopt(listen_skt, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
#endif
    
  
    set_blocking( listen_skt, false );
    
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
