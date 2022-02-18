#include "c11_explicit_class.h"
#include <stdio.h>



IntegerTest::IntegerTest( int a )
    : data{a}
{}




void IntegerTest::print()
{
    printf("data = %d\n", data );
}




namespace c11 {


void test_explicit_class()
{
    // constructor如果不只一個變數, 會造成編譯錯誤
    // 宣告加上 explicit 後這邊編譯不過
    //IntegerTest itest = 1.33;
    IntegerTest itest{3};

    itest.print();
}


} // end namespace c11