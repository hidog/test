#include "static_lib_1.h"
#include <iostream>
#include <string>
#include <cassert>


StaticObj::StaticObj()
{
    std::cout << "StaticObj constructor" << std::endl;
}


StaticObj::~StaticObj()
{
    std::cout << "StaticObj destructor" << std::endl;
}


int StaticObj::test_func()
{
    std::cout << "File = " << __FILE__ << "\nFunction = " << __FUNCTION__ << "\nline = " << __LINE__ << std::endl;
    return 4;
}


int static_lib_func_1()
{
#ifdef UNIX
    assert(0);
#endif

#ifdef _DEBUG
    std::string debug_release = "Debug";
#else
    std::string debug_release = "Release";
#endif

#ifdef _WIN32
    std::string platform = "Windows";
#elif defined(UNIX)
    std::string platform = "Linux";
#endif

    std::cout << "This is static lib test" << std::endl;
    std::cout << "File = " << __FILE__ << "\nFunction = " << __FUNCTION__ << "\nline = " << __LINE__ << std::endl;
    std::cout << debug_release << " build, OS = " << platform << std::endl;

    return STATIC_LIB;
}


