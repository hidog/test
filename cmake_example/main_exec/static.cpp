#include "static.h"

#include "static_lib_1.h"
#include "static_lib_2.h"

// 不加extern "C" 會造成mangling錯誤
extern "C" {
#include "static_lib_3.h"
}

#include <iostream>


using namespace std;




void static_test()
{
    int ret;

    // static lib test.
    ret = static_lib_func_1();
    cout << "ret = " << ret << endl;

    static_lib_func_2();
    cout << endl;

    static_lib_func_3();
    cout << endl;

    StaticObj s_obj;
    s_obj.test_func();
    cout << endl;
}
