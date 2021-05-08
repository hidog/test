#ifndef UDP_EXAMPLE_2_H
#define UDP_EXAMPLE_2_H


#ifdef _WIN64
unsigned __int64 get_udp_socket( const char*ip, const char* port );
#else
int get_udp_socket( const char* ip, const char* port );
#endif

void udp_hello_server_2( const char* port );
void udp_hello_client_2( const char* ip, const char* port );

// 測試UDP掉封包問題
void udp_test_package_loss_server(void);
void udp_test_package_loss_client(void);

// 多個client架構, 會根據ip計算目前幾個人連進來
void udp_multi_server(void);
void udp_multi_client(void);

// select同時判斷read/write
void udp_nonblocking_2(void);


#endif
