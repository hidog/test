#include "static_lib_2.h"
#include <stdio.h>


int static_lib_func_3()
{
    printf( "\n\n* static_lib_func_3\n" );
    printf( "    file = %s\n    function = %s, line = %d\n", __FILE__, __func__, __LINE__ );
    printf( "    date = %s, time = %s\n", __DATE__, __TIME__ );
    
    return 3;
}


