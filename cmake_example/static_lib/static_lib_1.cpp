#include "static_lib_1.h"
#include <iostream>
#include <string>
#include <cassert>

using namespace std;


StaticObj::StaticObj()
{}


StaticObj::~StaticObj()
{}


int StaticObj::test_func()
{
    std::cout << "\n\n* StaticObj::test_func" << endl;
    std::cout << "    File = " << __FILE__ << endl
              << "    Function = " << __FUNCTION__ << endl
              << "    line = " << __LINE__ << endl;
    return 4;
}





int static_lib_func_1()
{
    cout << "\n\n* static_lib_func_1" << endl;
    
#ifdef UNIX
    //assert(0);
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
#elif defined(MACOS)
    std::string platform = "MACOS";
#endif
    
#ifdef TEST_DEBUG
    cout << "    define TEST_DEBUG" << endl;
#endif

#ifdef NDEBUG
    cout << "    define _NDEBUG" << endl;
#endif
    

    cout << "    This is static lib test" << endl;
    cout << "    File = " << __FILE__ << endl 
         << "    Function = " << __FUNCTION__ << endl 
         << "    line = " << __LINE__ << endl;
    cout << "    " << debug_release << " build, OS = " << platform << endl;
    cout << "    STATIC_LIB = " << STATIC_LIB << endl;
    
    return STATIC_LIB;
}


