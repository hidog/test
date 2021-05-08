#include <iostream>
#include "../../udp_example.h"
#include "../../tool.h"
#include "../../tcp_example.h"

extern "C" {
#include "../../tool_2.h"
#include "../../udp_example_2.h"
#include "../../tcp_example_2.h"
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




int main()
{
    //transform_test();
    //test_getaddrinfo();
    //error_handle_test();

    //sockopt_test();

    //tcp_hello_server( 8888 );
    //tcp_hello_client( "192.168.1.112", 8888 );

    //tcp_hello_server_2( 7777 );

    tcp_server_timeout_test();

    system("PAUSE");
    return 0;
}

