#include <iostream>

#include "../../udp_example.h"
#include "../../tool.h"
#include "../../tcp_example.h"
#include "../../tcp_nb.h"
#include "../../p2p_example.h"

extern "C" {
#include "../../tool_2.h"
#include "../../udp_example_2.h"
#include "../../tcp_example_2.h"
#include "../../tcp_nonblock.h"
}



void udp_test_funcs()
{
    //udp_error_test();
    
    //udp_hello_server( 12349 );
    //udp_hello_client( std::string("127.0.0.1"), 12349 );

    // 122.116.84.59
    //udp_hello_server_2( "12349" );
    //udp_hello_client_2( "127.0.0.1", "12349" );

    //udp_test_package_loss_server();
    //udp_test_package_loss_client();

    //udp_RTT_server();
    //udp_RTT_client();

    //udp_multi_server();
    //udp_multi_client();

    //udp_nonblocking_server();
    //udp_nonblockint_client();

    //udp_nonblocking_2();
    //udp_broadcast_sender();
    //udp_broadcast_recver();

    //udp_package_order_client();
}



void tool_test_funcs()
{
    //transform_test();
    //test_getaddrinfo();
    //error_handle_test();

    //sockopt_test();
    //get_domain_name();
    //get_local_ip();
    get_mac_address();
}



void tcp_test_funcs()
{
    //tcp_hello_server( 8888 );
    //tcp_hello_client( "192.168.1.112", 8888 );

    //tcp_hello_server_2( 1234 );
    //tcp_hello_client_2( "192.168.1.112", 1234 );

    //tcp_server_timeout_test();
    //tcp_client_timeout_test();

    //tcp_size_server( 2318 );
    //tcp_size_client( "122.116.84.59", 2318 );

    tcp_server_non_blocking( 1234 );
    //tcp_client_blocking( "192.168.1.108", 1234 );
    //tcp_client_non_blocking( "192.168.1.106", 1234 );

    //TcpNb tcp_nb("Slave PC", 1234);
    //tcp_nb.work();
}



int main()
{
    //tool_test_funcs();
    //tcp_test_funcs();

    //p2p_server_1();
    p2p_server_2();
    //p2p_server_1();
    p2p_client_2();

    return 0;
}

