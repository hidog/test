#include <iostream>

#include "../../tool.h"
#include "../../udp_example.h"

extern "C" {
#include "../../tool_2.h"
#include "../../udp_example_2.h"
}


int main(int argc, const char * argv[])
{
    //transform_test();
    //test_getaddrinfo();
    
    //udp_hello_server(1234);
    //udp_hello_client( std::string("127.0.0.1"), 1234 );

    //udp_package_order_server();
    udp_package_order_client();
    
    return 0;
}
