#ifndef TCP_NB_H
#define TCP_NB_H

#include <vector>
#include <string>


#if defined(MACOS) || defined(UNIX)
typedef int SOCKET;
#define INVALID_SOCKET -1
#endif




struct ClientSocket
{
    SOCKET skt;
    bool connected;
    
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
    
private:
    
    const int port = 1234;
    SOCKET listen_skt = INVALID_SOCKET;
    std::vector<std::string> ip_list;
};



#endif
