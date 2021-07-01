#ifndef DYNAMIC_LIB_1_H
#define DYNAMIC_LIB_1_H

#include "def.h"

/*
    測試macro
    測試動態載入dll func
    導入其他library
*/


/* 
    windows平台不加 dll export, 會造成編譯錯誤.
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



#endif
