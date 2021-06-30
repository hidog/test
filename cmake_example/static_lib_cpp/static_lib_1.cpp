#include "static_lib_1.h"
#include <stdio.h>

int static_lib_func_1()
{
#ifdef WIN32
    printf("this is win32 static lib cpp test.\n");
#endif

    return 1;
}


