#include "test.h"
#include "test2.h"
#include <stdio.h>

int main()
{
    int a = test();
    printf("a = %d\n", a);
    
    int b = test2();
    printf("b = %d\n", b);
    
#ifdef HIDOG_DEFINE
    printf( "define HIDOG_DEFINE\n" );
#endif

#ifdef UNIX
    printf( "define UNIX\n" );
#endif

    return 0;
}
