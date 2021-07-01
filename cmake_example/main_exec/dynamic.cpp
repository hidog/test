#include "dynamic.h"
#include "dynamic_lib_1.h"
#include <iostream>

#ifdef WIN32
#include <windows.h>
#elif defined(MACOS) | defined(UNIX)
#include "dlfcn.h"
#else
# error. undefined operator system.
#endif

using namespace std;


typedef int (*DllFunc_1)(void);






void explicit_load_func()
{
#ifdef WIN32
    HINSTANCE hDLL;
    hDLL = LoadLibrary("dynamic_lib");
    if( hDLL != NULL )
        cout << "load sucess!!\n" ;
    else
    {
        cout << "load fail.\n" ;
        return;
    }

    DllFunc_1 dll_func_1 = NULL;

    dll_func_1 = (DllFunc_1)GetProcAddress( hDLL, "dynamic_func_1" );
    if( dll_func_1 != NULL )
        cout << "get func point sucess\n" ;
    else
    {
        cout << "Get func point fail.\n" ;
        return;
    }

    int ret = dll_func_1();
    printf( "ret = %d\n", ret );

    FreeLibrary(hDLL);

#elif  defined(MACOS) | defined(UNIX)
    void        *handle;
    DllFunc     func;

    handle  =   dlopen( "libdlldyn.so", RTLD_LAZY );
    if( handle )
        cout << "open .so success\n";
    else
        cout << "open .so fail\n";

    func    =   (DllFunc)dlsym( handle, "dll_dynamic_message" );
    if( func )
        cout << "load func success\n";
    else
        cout << "load func fail\n";

    char    str[100]    =   "load .so dynamic.\n";
    func( str );
#else
# error. undefined operator system.
#endif
}





void dynamic_test()
{
    // dynamic lib test.
    int b = dynamic_func_1();

    DynamicObj d_obj;
    d_obj.test_func();

}
