#include <iostream>
#include "../../udp_example.h"
#include "../../tool.h"

extern "C" {
#include "../../tool_2.h"
#include "../../udp_example_2.h"
}



int main()
{
    //transform_test();
    //test_getaddrinfo();
        
   //udp_hello_server( 12349 );
    //udp_hello_client( std::string("127.0.0.1"), 12349 );
    // 122.116.84.59
    //udp_hello_server_2( "12349" );
    //udp_hello_client_2( "127.0.0.1", "12349" );

    //udp_package_order_client();s
    //error_handle_test();

    //udp_test_package_loss_server();
    //udp_test_package_loss_client();
    //sockopt_test();

    //udp_RTT_server();
    //udp_RTT_client();

    //udp_multi_server();
    //udp_multi_client();

    //udp_nonblocking_server();
    //udp_nonblockint_client();

    udp_nonblocking_2();

    system("PAUSE");
    return 0;
}

