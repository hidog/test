#include "static_lib_2.h"
#include <stdio.h>


int static_lib_func_3()
{
    printf( "file = %s, function = %s, line = %d\n", __FILE__, __FUNCTION__, __LINE__ );

    return 3;
}


