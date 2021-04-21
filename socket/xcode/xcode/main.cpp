#include <iostream>
#include "../../tool.h"
#include "../../udp_example.h"

int main(int argc, const char * argv[])
{
    transform_test();
    
    //udp_hello_server(1234);
    udp_hello_client( std::string("127.0.0.1"), 1234 );
    
    return 0;
}
