#include "dynamic_lib_1.h"
#include <iostream>
#include <cassert>

using namespace std;



DynamicObj::DynamicObj()
{}



DynamicObj::~DynamicObj()
{}




void DynamicObj::test_func()
{
    // 推薦使用 __func__
    cout << "\n\n* test_func" << endl;
    cout << "    file = " << __FILE__ << endl 
#ifdef WIN32
        << "    name = " << __FUNCSIG__ << endl
#else
        << "    name = " << __PRETTY_FUNCTION__ << endl
#endif
         << "    function = " << __FUNCTION__ << endl 
         << "    func = " << __func__ << endl
         << "    line = " << __LINE__ << endl;
}




int dynamic_func_1()
{
    cout << "\n\n* dynamic_func_1" << endl;
    
#ifdef UNIX
    //assert(0);
#endif

#ifdef __cplusplus
    int cpp_def = __cplusplus;
#else
    int cpp_def = -1;
#endif

/*
    XCODE環境下, __STDC__ 有作用
    雖然是cpp檔案卻仍有定義, 跟原本猜測的不同
*/
#ifdef __STDC__
    int c_def = __STDC__; 
#else
    int c_def = -1;
#endif
    
    cout << "    cpp_def = " << cpp_def << ", c_def = " << c_def << endl;

#ifdef DEBUG
    cout << "    defined DEBUG" << endl;
#endif
    
#ifdef NDEBUG 
    cout << "    defined NDEBUG" << endl;
#endif
    
    cout << "    DYNAMIC_LIB = " << DYNAMIC_LIB << endl;

    return DYNAMIC_LIB;
}




int dynamic_func_4()
{
    cout << "\n\n* dynamic_func_4" << endl;
    cout << "    This function allow load explicitly." << endl;
    return 8195;
}

