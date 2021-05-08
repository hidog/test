#include <iostream>
#include "../udp_example.h"
#include "../tcp_example.h"
#include "../tool.h"

extern "C"
{
#include "../udp_example_2.h"
#include "../tcp_example_2.h"
#include "../tool_2.h"
}



int udp_test_funcs()
{
    //udp_error_test();

    //udp_hello_server( 12345 );
    //udp_hello_client( std::string("127.0.0.1"), 12345 );

    //udp_hello_server_2( "12345" );
    //udp_hello_client_2( "127.0.0.1", "12345" );

    //udp_test_package_loss_client();
    //udp_test_package_loss_server();

    //udp_RTT_client();
    //udp_RTT_server();

    //udp_multi_client();
    //udp_multi_server();

    //udp_nonblocking_2();

    //udp_broadcast_recver();
    //udp_broadcast_sender();

}




int main()
{
    udp_test_funcs();

    //transform_test();
    //test_getaddrinfo();
    //error_handle_test();
    //sockopt_test();

    //tcp_hello_server(8888);
    //tcp_hello_client( "192.168.1.112", 8888 );
    
    //tcp_client_timeout_test();
    tcp_server_timeout_test();

    return 0;
}
