#ifndef TCP_NB_H
#define TCP_NB_H

#include <vector>
#include <string>
#include <chrono>

#if defined(MACOS) || defined(UNIX)
#include <sys/select.h>
#endif


#if defined(MACOS) || defined(UNIX)
typedef int SOCKET;
#define INVALID_SOCKET -1
#endif



using namespace std::chrono;



struct ClientSocket
{
    SOCKET skt;
    bool connected;
    time_point<system_clock,milliseconds> connect_time;
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
    
private:
    
    const int port = 1234;
    SOCKET listen_skt = INVALID_SOCKET;
    std::vector<std::string> ip_list;
    std::vector<ClientSocket> client_list;
    
    fd_set w_set, r_set;
    SOCKET max_skt;
};



#endif
