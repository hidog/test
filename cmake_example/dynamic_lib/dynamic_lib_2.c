#include "dynamic_lib_2.h"
#include <stdio.h>



double dynamic_func_2( int a, int b )
{
    printf("\n\n* dynamic_func_2\n");
    printf("    file = %s\n    function = %s, line = %d\n", __FILE__, __func__, __LINE__ );
    printf("    date = %s, time = %s\n", __DATE__, __TIME__ );

#ifdef __cplusplus
    int cpp_def = __cplusplus;
#else
    int cpp_def = 0;
#endif

#ifdef __STDC__
    int c_def = __STDC__;
#else
    int c_def = 0;
#endif

    printf("    cpp_def = %d, c_def = %d\n", cpp_def, c_def );

    if( b == 0 )
        b = 1;
    return 1.0 * a / b;
}


