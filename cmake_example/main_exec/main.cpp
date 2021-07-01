#include "static.h"
#include "dynamic.h"
#include <cstdlib>

#include <cassert>
#include <stdio.h>


int main()
{
    printf("cmake test.\n");

#ifdef DEBUG
    printf("This is debug version.\n");
#endif

    static_test();

    dynamic_test();

    explicit_load_func();



#ifdef UNIX
    /*
        UNIX環境下
        cmake -DCMAKE_BUILD_TYPE=Debug .
        此時會跳assert.
    */
    assert(0);
#endif




#ifdef _WIN32
    system("PAUSE");
#endif

    return 0;
}
    
