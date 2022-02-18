#ifndef DYNAMIC_LIB_1_H
#define DYNAMIC_LIB_1_H

#include "def.h"


/* 
    windows平台不加 dll export, 會造成編譯錯誤.

    幾個macro參考.
    https://docs.microsoft.com/zh-tw/cpp/preprocessor/predefined-macros?view=msvc-160
*/



class LIBRARY_API DynamicObj
{
public:
    DynamicObj();
    ~DynamicObj();

    void test_func();
private:
};



LIBRARY_API int dynamic_func_1();


/* 
    要能夠explicit載入, 則需要加入extern "C"
    不然會發生錯誤.
*/
extern "C" {
LIBRARY_API int dynamic_func_4();
}




#endif
