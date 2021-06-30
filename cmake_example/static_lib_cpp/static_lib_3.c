#include "static_lib_2.h"
#include <stdio.h>


int static_lib_func_3()
{
#ifdef _DEBUG
    char debug_release[100] = "Debug";
#else
    char debug_release[100] = "Release";
#endif

#ifdef _WIN32
    char platform[100] = "Windows";
#endif

    printf( "test for pure C with out extern C.\n" );   

    return 1;
}


