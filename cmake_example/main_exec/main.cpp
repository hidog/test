#include "dynamic_lib_1.h"
#include <stdio.h>

int main()
{
    int a = dynamic_func_1();
    printf("a = %d\n", a);
    
    int b = dynamic_func_1();
    printf("b = %d\n", b);
    
#ifdef HIDOG_DEFINE
    printf( "define HIDOG_DEFINE\n" );
#endif

#ifdef UNIX
    printf( "define UNIX\n" );
#endif

    return 0;
}
