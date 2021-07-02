#include "static.h"
#include "dynamic.h"
#include "explicit_load.h"

#include <cstdlib>
#include <cassert>
#include <stdio.h>


int main()
{
    printf("*** cmake test.\n");

#ifdef DEBUG
    printf("*** This is debug version.\n");
#elif defined(NDEBUG)
    printf("*** This is release version.\n");
#else
//#error not define debug or release
#endif

    static_test();

    dynamic_test();

    explicit_load_func();

    printf("\n\nMAIN_EXEC = %d\n", MAIN_EXEC );

#ifdef UNIX
/*
    UNIX環境下
    預設是debug build, 會跳assert.
    調成預設是Release build.
 
    可用底下命令作切換
    cmake -DCMAKE_BUILD_TYPE=Debug .
    cmake -DCMAKE_BUILD_TYPE=Release .
*/
    //assert(0);
#endif

#ifdef WIN32
    printf( "\n\nversion = %d\n\n", _MSC_FULL_VER );
#else
    printf( "\n\nversion = %s\n\n", __VERSION__ );
#endif



/*
    參考 https://jyhshin.pixnet.net/blog/post/26588145
 #if __STDC_VERSION__ < 199901L  
 # if __GNUC__ >= 2  
 #  define __func__ __FUNCTION__  
 # else  
 #  define __func__ ""  
 # endif  
 #endif
*/
    
    
#ifdef TEST_RELEASE
    printf("\n\n define TEST_RELEASE\n\n" );
#endif


#ifdef _WIN32
    system("PAUSE");
#endif

    return 0;
}
    
