#include <iostream>

#include "../../tool.h"
#include "../../udp_example.h"
#include "../../tcp_example.h"
#include "../../tcp_nb.h"

extern "C" {
#include "../../tool_2.h"
#include "../../udp_example_2.h"
#include "../../tcp_example_2.h"
#include "../../tcp_nonblock.h"
}





void udp_funcs()
{
    //udp_error_test();
    
    //udp_hello_server(1234);
    //udp_hello_client( std::string("127.0.0.1"), 1234 );

    //udp_package_order_server();
    //udp_package_order_client();
    
    //udp_RTT_server();
    //udp_RTT_client();
    
    //udp_multi_client();
    //udp_multi_server();
    
    //udp_nonblocking_server();
    //udp_nonblockint_client();
    
    //udp_nonblocking_2();
    
    //udp_broadcast_recver();
    //udp_broadcast_sender();
}



void tool_funcs()
{
    //transform_test();
    //test_getaddrinfo();
    
    //sockopt_test();
    
    get_domain_name();
    get_mac_address();
    get_local_ip();
    get_local_ip_2();
    get_local_ip_3();
}




void tcp_funcs()
{
    //tcp_hello_server(8888);
    //tcp_hello_client( "192.168.1.101", 8888 );
    
    //tcp_hello_server_2(1234);
    //tcp_hello_client_2( "192.168.1.108", 1234 );
    
    //tcp_client_timeout_test();
    
    //tcp_size_server(2912);
    
    //tcp_server_non_blocking(1234);
    //tcp_client_non_blocking( "192.168.1.102", 1234 );
    //tcp_client_blocking( "192.168.1.102", 1234 );
    
    TcpNb tcp_nb("imac", 1234);
    tcp_nb.work();
}




int main(int argc, const char * argv[])
{
    tool_funcs();
    //tcp_funcs();
    
    return 0;
}
