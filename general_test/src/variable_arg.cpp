#include "variable_arg.h"
#include <cstdarg>
#include <stdio.h>



void va_test( int n, ... )
{
    int     i;
    int     value;
    
    va_list vl;
    va_start( vl, n );
    
    for( i = 0; i < n; i++ )
    {
        value = va_arg( vl, int );
        printf( "list[%d] = %d\n", i, value );
    }
    
    va_end(vl);
    printf ("\n");
}



