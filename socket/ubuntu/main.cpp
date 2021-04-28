#include <iostream>
#include "../udp_example.h"
#include "../tool.h"

extern "C"
{
#include "../udp_example_2.h"
#include "../tool_2.h"
}

int main()
{
    // transform_test();
    //test_getaddrinfo();

    //udp_hello_server( 12345 );
    //udp_hello_client( std::string("127.0.0.1"), 12345 );
    
    //udp_hello_server_2( "12345" );
    //udp_hello_client_2( "127.0.0.1", "12345" );
    
    //error_handle_test();
    //udp_test_package_loss_client();
    //udp_test_package_loss_server();
    
    sockopt_test();

    return 0;
}
