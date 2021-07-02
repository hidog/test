#include "explicit_load.h"

#include <iostream>

// use for explicit load dll.
#ifdef WIN32
#include <windows.h>
#elif defined(MACOS) | defined(UNIX)
#include "dlfcn.h"
#else
# error. undefined operator system.
#endif



// use for explicit load dll.
typedef int (*DllFunc_4)(void);
typedef double (*DllFunc_2)(int,int);
typedef void (*DllFunc_3)(void);



using namespace std;




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
    
    //
    void *handle;

#ifdef UNIX
    handle = dlopen( "libdynamic_lib.so", RTLD_LAZY );
#elif defined(MACOS)
    handle = dlopen( "libdynamic_lib.dylib", RTLD_LAZY );
#else
#error undefined.
#endif
    
    if( handle )
        cout << "open .so or .dylib success\n";
    else
        cout << "open .so or .dylib fail\n";

    //
    DllFunc_4 func_4;
    func_4 = (DllFunc_4)dlsym( handle, "dynamic_func_4" );
    if( func_4 )
        cout << "load func success\n";
    else
        cout << "load func fail\n";
    int ret = func_4();
    printf( "ret = %d\n\n", ret );
    
    //
    DllFunc_2 func_2;
    func_2 = (DllFunc_2)dlsym( handle, "dynamic_func_2" );
    if( func_2 )
        cout << "load func success\n";
    else
        cout << "load func fail\n";
    double ret2 = func_2(113,552);
    printf( "ret2 = %lf\n\n", ret2 );
    
    //
    DllFunc_3 func_3;
    func_3 = (DllFunc_3)dlsym( handle, "dynamic_func_3" );
    if( func_3 )
        cout << "load func success\n";
    else
        cout << "load func fail\n";
    func_3();

    
    //
    dlclose(handle);
    
#else
# error. undefined operator system.
#endif
}
