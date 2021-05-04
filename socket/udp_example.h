#ifndef UDP_EXAMPLE_H
#define UDP_EXAMPLE_H

#include <string>

void udp_bind_client_socket( int skt );

void udp_hello_server( int port );
void udp_hello_client( std::string ip, int port );
void udp_hello_client_c( std::string ip, int port );

void udp_package_order_server();
void udp_package_order_client();

void udp_RTT_server(); // RTT = Round Trip Time (RTT)
void udp_RTT_client();

void udp_nonblocking_server();
void udp_nonblockint_client();

void udp_broadcast_sender();
void udp_broadcast_recver();

/*
    還需要做同時send/recv的non-blocking
    想像一個thread,同時可以收音後發放,跟接收對面的語音.
 */


/*
    * 1. 測量RTT (封包送達時間) 
    * 2. 測試掉封包 
    * 3. 測試封包順序問題   
    * 4. 測試一個server多個client
    * 5. 設置timeout
    * 6. 設置non-blocking
    7. 廣播
 */


#endif
