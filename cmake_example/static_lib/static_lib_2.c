#include "static_lib_2.h"
#include <stdio.h>


int static_lib_func_2()
{
#ifdef _DEBUG
    char debug_release[100] = "Debug";
#else
    char debug_release[100] = "Release";
#endif

#ifdef _WIN32
    char platform[100] = "Windows";
#elif defined(UNIX)
    char platform[100] = "Linux";
#endif

    printf( "test for pure C.\n" );   
    printf( "build type = %s, platform = %s\n", debug_release, platform );
    printf( "file = %s\nfunction = %s\nline = %d\n", __FILE__, __FUNCTION__, __LINE__ );

    return 2;
}


