#include <iostream>
#include "../../udp_example.h"
#include "../../tool.h"

int main()
{
    transform_test();
        
    //udp_hello_server( 12345 );
    udp_hello_client( std::string("122.116.84.59"), 12345 );
    // 122.116.84.59

    system("PAUSE");
    return 0;
}

