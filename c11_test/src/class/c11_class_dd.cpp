#include "c11_class_dd.h"

#include <iostream>

using namespace std;


void ClassDelete_1::test()
{
    cout << "This is class delete test 1." << endl;
}




ClassDelete_2::ClassDelete_2() 
    : data{10} 
{}



ClassDelete_2::ClassDelete_2( const ClassDelete_2& c ) 
{ 
    data = c.data + 1; 
}



ClassDelete_2& ClassDelete_2::operator = ( const ClassDelete_2& c )
{
    data = c.data + 1;
    return *this;
}



void ClassDelete_2::print() 
{
    cout << "data = " << data << endl;
}








void ClassDefault::init()
{
    data = new int[10];
    for( int i = 0; i < 10; i++ )
        data[i] = i + 1;
}




void ClassDefault::print()
{
    for( int i = 0; i < 10; i++ )
        cout << data[i] << ", ";
    cout << endl;
}






#if 1
ClassDelete_2& test_delete_func( ClassDelete_2 a )
{
    // ubuntu環境下在這邊發生記憶體錯誤. (代表釋放的時間跟其他平台不同)
    // 只是為了測試class delete, 直接改用不會crash的寫法
    static ClassDelete_2 b = a;
    return b;
}
#else
ClassDelete_2 test_delete_func( ClassDelete_2 a )
{
    // move constructor 設為 delete, 會造成這個函數編譯錯誤. 
    return a; 
}
#endif







namespace c11 {



void class_delete_test_1()
{
    cout << "class_delete_test_1" << endl;
    ClassDelete_1 c1, c2;
    //c1 = c2; // 會導致編譯錯誤,因為 = 設為delete
    c1.test();
    c2.test();
}








void class_delete_test_2()
{
    ClassDelete_2 c;
    ClassDelete_2 c2;
    c2 = test_delete_func(c);
    c2.print();
}



void class_default_test()
{
    ClassDefault d1, d2;
    d1.init();
    d2 = d1;

    d1.print();
    d2.print();

    // 兩邊的pointer指向位址相同. 使用default的情況下只會複製指標位置.
    // 因此無法用destructor釋放記憶體
    cout << d1.data << " " << d2.data << endl;
    delete [] d1.data;
}





void class_dd_funcs()
{
    class_delete_test_1();
    class_delete_test_2();

    class_default_test();
}





} // end namespace c11 
