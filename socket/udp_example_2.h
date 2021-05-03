#ifndef UDP_EXAMPLE_2_H
#define UDP_EXAMPLE_2_H


int get_udp_socket( const char* ip, const char* port );

void udp_hello_server_2( const char* port );
void udp_hello_client_2( const char* ip, const char* port );

void udp_test_package_loss_server(void);
void udp_test_package_loss_client(void);

void udp_multi_server(void);
void udp_multi_client(void);

void udp_nonblocking_2(void);


#endif
