#include "lr_func.h"
#include <iostream>

using namespace std;



int& left_value()
{
    static int l_value = 0;
    return l_value;
}






// 用來測試編譯失敗的函數.
int right_value()
{
    static int r_value = 3;
    cout<< r_value << endl;
    return r_value;
}







void ref_test_1( int &x )
{
    cout << ++x << endl;
}





void ref_test_2( const int&x )
{
    cout << x << endl;
}





void ref_test_3( int&& x)
{
    cout << ++x << endl;
}





int ref_test_4()
{
    return  -4723;
}







void lr_test_f1( int&& a )
{
    cout << a << endl;
    a += 111;
    cout << a << endl;
    cout << "addr a = " << &a << endl;
    // a 能取址,變成一個左值
}


