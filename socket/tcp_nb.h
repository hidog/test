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


#if defined(MACOS) || defined(UNIX)
typedef int SOCKET;
#define INVALID_SOCKET -1
#endif



using namespace std::chrono;



enum class PacketType
{
    UNKNOWN,
    SHORT,
    LONG
};



enum class SendState
{
    UNKNOWN,
    HEADER,
    BODY,
    STOP, // for test use.
};






struct PacketHeader
{
    char name[24];
    PacketType type;
    int size;
};
const int HEADER_SIZE = sizeof(PacketHeader);


struct PacketShort
{
    int count;
    char message[100];
};
const int PACKET_SHORT_SIZE = sizeof(PacketShort);


struct PacketLong
{
    int count;
    char data[4096];
};




union PacketBody
{
    PacketShort short_data;
    PacketLong long_data;
};
const int BODY_SIZE_LONG = sizeof(PacketLong);
const int BODY_SIZE_SHORT = sizeof(PacketShort);


struct SendData
{
    PacketHeader header;
    PacketBody body;
    int send_index;
};



enum class RecvState
{
    UNKNOWN,
    HEADER,
    BODY,
    STOP, // test use.
};





struct RecvData
{
    PacketHeader header;
    PacketBody body;
    int recv_index;
};





struct ClientSocket
{
    unsigned long net_ip;
    SOCKET skt;
    bool connected;
    int task_count;    
    time_point<system_clock,milliseconds> connect_time;

    SendState send_state;
    SendData send_data;

    RecvState recv_state;
    RecvData recv_data;

};




class TcpNb
{
public:
    TcpNb();
    ~TcpNb();
    
    int init();
    int work();
    
    int setup_server_skt();
    int set_non_blocking( SOCKET skt, bool enable );
    void add_fd_set();
    SOCKET get_max_skt();
    int get_error_code();

    void recv_handle();
    void send_handle();
    
    void handle_disconnect( std::vector<ClientSocket>::iterator clt );
    void handle_error( std::vector<ClientSocket>::iterator clt );

    int find_client( unsigned long net_ip );
    void connect_to( const char* const ip, int port );

    void prepare_send_data( ClientSocket &client );
    void send_header( ClientSocket &client );
    void send_body( ClientSocket &client );

    void accept_handle();
    void prepare_recv_data( ClientSocket &client );
    void recv_header( ClientSocket &client );
    void recv_body( ClientSocket &client );

private:
    
    const int port = 1234;
    SOCKET listen_skt = INVALID_SOCKET;
    std::vector<std::string> ip_list;
    std::vector<ClientSocket> client_list;
    
    fd_set w_set, r_set;
    SOCKET max_skt;
};



#endif
