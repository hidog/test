#include "c11_mutable_class.h"
#include <stdio.h>



DoubleTest::DoubleTest()
{}




void DoubleTest::add_one() const
{
    data += 1;
    //data2 += 1;   //不是mutable變數,會造成編譯錯誤.
}




void DoubleTest::print()
{
    printf("data = %lf\n", data );
}




namespace c11 {


void test_mutable_class()
{
    DoubleTest dt;
    dt.add_one();
    dt.print();
}


} // end namespace c11