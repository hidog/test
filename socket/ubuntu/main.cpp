#include <iostream>
#include "../udp_example.h"
#include "../tcp_example.h"
#include "../tool.h"
#include "../tcp_nb.h"

extern "C"
{
#include "../udp_example_2.h"
#include "../tcp_example_2.h"
#include "../tcp_nonblock.h"
#include "../tool_2.h"
}



void udp_test_funcs()
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





void tool_funcs()
{
    //transform_test();
    //test_getaddrinfo();
    //error_handle_test();
    //sockopt_test();
    get_domain_name();
    get_local_ip_2();
    get_local_ip_3();
    get_mac_address();
}





void tcp_funcs()
{
    //tcp_hello_server(8888);
    //tcp_hello_client( "192.168.1.112", 8888 );
    
    //tcp_hello_server_2( 1234 );
    //tcp_hello_client_2( "192.168.1.102", 1234 );

    //tcp_client_timeout_test();
    //tcp_server_timeout_test();
    
    //tcp_size_server( 2591 );
    //tcp_size_client( "36.231.96.77", 2912 );
    
    //tcp_server_non_blocking( 1234 );
    //tcp_client_non_blocking( "127.0.0.1", 1234 );
    tcp_client_blocking( "111.248.199.3", 1234 );
    
    //TcpNb tcp_nb("UB red", 1235 );
    //tcp_nb.work();
}





int main()
{
    //tool_funcs();
    //udp_test_funcs();
    tcp_funcs();

    return 0;
}
