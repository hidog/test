#include "dynamic.h"

#include "dynamic_lib_1.h"
#include "dynamic_lib_2.h"

extern "C" {
#include "dynamic_lib_3.h"
}


#include <iostream>


// use for explicit load dll.
#ifdef WIN32
#include <windows.h>
#elif defined(MACOS) | defined(UNIX)
#include "dlfcn.h"
#else
# error. undefined operator system.
#endif

using namespace std;

// use for explicit load dll.
typedef int (*DllFunc_4)(void);
typedef double (*DllFunc_2)(int,int);
typedef void (*DllFunc_3)(void);


void explicit_load_func()
{
#ifdef WIN32

    //
    HINSTANCE hDLL;
    hDLL = LoadLibrary("dynamic_lib");
    if( hDLL != NULL )
        cout << "load sucess!!\n" ;
    else
    {
        cout << "load fail.\n" ;
        return;
    }

    //
    DllFunc_4 dll_func_4 = NULL;

    dll_func_4 = (DllFunc_4)GetProcAddress( hDLL, "dynamic_func_4" );
    if( dll_func_4 != NULL )
        cout << "get func point sucess\n" ;
    else
    {
        cout << "Get func point fail.\n" ;
        return;
    }

    int ret = dll_func_4();
    printf( "ret = %d\n\n", ret );

    //
    DllFunc_2 dll_func_2 = NULL;

    dll_func_2 = (DllFunc_2)GetProcAddress( hDLL, "dynamic_func_2" );
    if( dll_func_2 != NULL )
        cout << "get func point sucess\n" ;
    else
    {
        cout << "Get func point fail.\n" ;
        return;
    }

    double ret2 = dll_func_2(97,101);
    printf( "ret2 = %lf\n\n", ret2 );

    //
    DllFunc_3 dll_func_3 = NULL;

    dll_func_3 = (DllFunc_3)GetProcAddress( hDLL, "dynamic_func_3" );
    if( dll_func_3 != NULL )
        cout << "get func point sucess\n" ;
    else
    {
        cout << "Get func point fail.\n" ;
        return;
    }
    dll_func_3();
    printf("\n\n");

    //
    FreeLibrary(hDLL);

#elif  defined(MACOS) | defined(UNIX)
    /*void        *handle;
    DllFunc_1   func;

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
    func( str );*/
#else
# error. undefined operator system.
#endif
}





void dynamic_test()
{
    int ret;
    
    //
    ret = dynamic_func_1();
    cout << "ret = " << ret << "\n\n";

    //
    double ret2 = dynamic_func_2( 481421, 9982157 );
    cout << "ret2 = " << ret2 << "\n\n";

    // 
    dynamic_func_3();
    
    // 
    ret = dynamic_func_4();
    cout << "ret = " << ret << "\n\n";

    

    DynamicObj d_obj;
    d_obj.test_func();

    cout << endl;

}
