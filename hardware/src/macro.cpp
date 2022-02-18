#include "macro.h"
#include <iostream>

using namespace std;


void x64_x86_macro()
{
// check x64 or x86

// Check windows
#if _WIN32 || _WIN64
#if _WIN64
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif


// check linux
#if __GNUC__
#   if __x86_64__ || __ppc64__
    // ubuntu 20, intel cpu, 會定義 __x86_64__
    // 其他CPU還有別的macro可用
        cout << "this is x64 macro\n";
#   else
        cout << "this is x86 macro\n";
#   endif
#endif

    if( sizeof(void*) == 8 )
        cout << "this is x64\n";
    else
        cout << "this is x86\n";

}


void c11_macro()
{
// c++11
#if __cplusplus <= 199711L
    cout << "less than c++11\n";
#else
    cout << "more than c++11\n";
#endif



#if __cplusplus==201402L
    std::cout << "C++14" << std::endl;
#elif __cplusplus==201103L
    std::cout << "C++11" << std::endl;
#else
    std::cout << "C++" << std::endl;
#endif

}
