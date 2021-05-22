#ifndef UDP_EXAMPLE_H
#define UDP_EXAMPLE_H

#include <string>

void udp_bind_client_socket( int skt );

void udp_hello_server( int port );
void udp_hello_client( std::string ip, int port );
void udp_hello_client_c( std::string ip, int port );

// 實驗封包順序,UDP未必照發送的順序收到訊息
void udp_package_order_server();
void udp_package_order_client();

// 測試封包往來的時間
void udp_RTT_server(); // RTT = Round Trip Time (RTT)
void udp_RTT_client();

// 測試non-block socket
void udp_nonblocking_server();
void udp_nonblockint_client();

// 測試廣播功能. 測試結果,同一個分享器底下的才有收到廣播封包. 是否能跨分享器呢? 有空再研究.
void udp_broadcast_sender();
void udp_broadcast_recver();

void udp_error_test();

#endif
