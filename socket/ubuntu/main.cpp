#include <iostream>
#include "../udp_example.h"
#include "../tool.h"

int main()
{
    transform_test();

    //udp_hello_server( 12345 );
    udp_hello_client( std::string("127.0.0.1"), 12345 );
    
    return 0;
}
