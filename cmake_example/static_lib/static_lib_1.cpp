#include "static_lib_1.h"
#include <iostream>
#include <string>


int static_lib_func_1()
{
#ifdef _DEBUG
    std::string debug_release = "Debug";
#else
    std::string debug_release = "Release";
#endif

#ifdef _WIN32
    std::string platform = "Windows";
#endif

    std::cout << "this is static lib cpp test" << std::endl;
    std::cout << debug_release << " build, OS = " << platform << std::endl;

    return 1;
}


