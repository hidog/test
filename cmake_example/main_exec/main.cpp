
#include "dynamic_lib_1.h"

#include "static_lib_1.h"
#include "static_lib_2.h"

// 不加extern "C" 會造成mangling錯誤
extern "C" {
#include "static_lib_3.h"
}

#include <iostream>



int main()
{
    int ret;

    // static lib test.
    ret = static_lib_func_1();
    std::cout << "ret = " << ret << "\n\n";

    static_lib_func_2();
    std::cout << std::endl;

    static_lib_func_3();
    std::cout << std::endl;

    StaticObj s_obj;
    s_obj.test_func();
    std::cout << std::endl;

    // dynamic lib test.
    int b = dynamic_func_1();

   
    return 0;
}
    