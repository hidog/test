#ifndef TCP_NB_H
#define TCP_NB_H

#include <vector>
#include <string>
#include <chrono>

#if defined(MACOS) || defined(UNIX)
#include <sys/select.h>
#else
#include <Winsock2.h>
#endif

#ifdef MACOS
#include <sys/types.h>
#endif


#if defined(MACOS) || defined(UNIX)
typedef int SOCKET;
#define INVALID_SOCKET -1
#endif

#ifdef MACOS
typedef in_addr_t NetAddr;
#elif defined(UNIX)
typedef uint32_t NetAddr;
#else
typedef unsigned long NetAddr;
#endif



#define LONG_DATA_SIZE 4096


using namespace std::chrono;

/*
    筆記: ubuntu環境下,socket有error的時候會被select標記為可讀寫,但send/recv後會報錯.
    這個範例同時支援 server/client, send/recv, connect/accept  non-blocking.
*/

enum class PacketType
{
    UNKNOWN,
    SHORT,
    LONG
};



enum class SendState
{
    UNKNOWN,
    HEAD,
    BODY,
    STOP, // for test use.
};






struct PacketHead
{
    char name[24];
    PacketType body_type;
    int body_size;
};
const int HEAD_SIZE = sizeof(PacketHead);


struct PacketShort
{
    int count;
    char message[100];
};



struct PacketLong
{
    int count;
    char data[LONG_DATA_SIZE];
};




union PacketBody
{
    PacketShort short_data;
    PacketLong long_data;
};
const int LONG_BODY_SIZE = sizeof(PacketLong);
const int SHORT_BODY_SIZE = sizeof(PacketShort);


struct SendData
{
    PacketHead head;
    PacketBody body;
    int send_index;
};



enum class RecvState
{
    UNKNOWN,
    HEAD,
    BODY,
    STOP, // test use.
};





struct RecvData
{
    PacketHead head;
    PacketBody body;
    int recv_index;
};





struct ClientSocket
{
    NetAddr net_ip;
    int port;
    SOCKET skt;
    bool connected;
    int task_count;    
    time_point<system_clock,milliseconds> connect_time;

    SendState send_state;
    SendData send_data;

    RecvState recv_state;
    RecvData recv_data;

};




struct Task
{
    std::string ip;
    int port;
    PacketType type;
    bool finished;
    time_point<system_clock,milliseconds> timestamp;
};




class TcpNb
{
public:
    TcpNb( std::string _pc_name, int _port );
    ~TcpNb();
    
    int init();
    int work();
    
    int setup_server_skt();
    int set_blocking( SOCKET skt, bool enable );
    void add_fd_set();
    SOCKET get_max_skt();
    int get_error_code();

    void connect_handle();     
    void recv_handle();
    void send_handle();
    
    void handle_disconnect( ClientSocket& client );
    void handle_error( ClientSocket& client );   

    bool is_exist_client( std::string ip, int port );
    int find_client( unsigned long net_ip, int port );
    void connect_to( const char* const ip, int port );

    void prepare_send_data( ClientSocket &client );
    void send_head( ClientSocket &client );
    void send_body( ClientSocket &client );

    void accept_handle();
    void prepare_recv_data( ClientSocket &client );
    void recv_head( ClientSocket &client );
    void recv_body( ClientSocket &client );

    void task_finish_handle();
    void handle_connect_timeout();

    void get_new_task();

private:
    
    int port;
    SOCKET listen_skt = INVALID_SOCKET;
    std::vector<std::string> ip_list;
    std::vector<int> port_list;
    std::vector<ClientSocket> client_list;
    
    fd_set w_set, r_set;
    SOCKET max_skt;

    std::string pc_name;
    Task task;

};



#endif
