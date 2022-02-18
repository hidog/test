#include "c11_lambda_op.h"
#include <iostream>
#include <functional>


using namespace std;








namespace c11 {



void lambda_operator_1()
{
    // 可以省略傳入參數的 ()
    auto func = [] {
        cout << "this is lambda_operator_1 test." << endl;
    };
    
    func();
}




void lambda_operator_2()
{
    // 回傳double.   -> double 表示return type
    // 此時就算沒有傳入引數, 也不能省略 ()
    auto func = [] ( int a, int b ) -> double {
        double r = 1.0 * a * b * 1.521; 
        return r;
    };
    
    double r = func(7,13);
    cout << "a * b * 1.521 = " << r << endl;
}



void lambda_operator_3()
{
    int a = 19, b = 32;
    
    auto func1 = [&a,b] () {
        a++;
        // b++;   // 未宣告 mutable的情況下,不能改變 b 的值
        return a + b;  // 一定情況下能自動推導回傳型別
    };
    
    // mutable 跟 -> int 是兩件事情
    // 一個控制call by value的變數可修改內容
    // 一個表示回傳值是int型態
    auto func2 = [&a,b] () mutable -> int {
        a++;
        b++;
        return a + b;
    };
    
    func1();
    cout << "a = " << a << ", b = " << b << endl;
    
    func2();
    cout << "a = " << a << ", b = " << b << endl;
}




void lambda_operator_4()
{
    int a = 1, b = 2, c = 3, d = 4, e = 5;
    
    // [&] 代表 a, b, c, ... local variable 用 reference 方式傳入
    auto func1 = [&] ( double x, double y ) -> double {
        a++;
        b++;
        c++;
        d++;
        e++;
        double r = a + b + c + d + e + x*y;
        return r;
    };
    
    double r1 = func1( 0.5, 0.6 );
    cout << "a = " << a << endl << "b = " << b << endl
         << "c = " << c << endl << "d = " << d << endl 
         << "e = " << e << endl << "r1 = " << r1 << endl;
    
    // c, d, e 用call by value方式傳入.   
    // mutable表示c,d,e可以修改值.
    // 但不影響function外的值
    auto func2 = [&,c,d,e] () mutable {
        a++;
        b++;
        c++;  // 這幾個 call by value
        d++;
        e++;
    };
    
    func2();
    cout << "a = " << a << endl << "b = " << b << endl
         << "c = " << c << endl << "d = " << d << endl 
         << "e = " << e << endl;
}




void lambda_operator_5()
{
    int a = 1, b = 2, c = 3, d = 4, e = 5;
    
    // [=] 代表 a, b, c, d, e用call by value方式傳入
    auto func1 = [=] ( double x, double y ) mutable -> double {
        a++;
        b++;
        c++;
        d++;
        e++;
        double r = 1.0*a + b + c + d + e + x*y;
        return r;
    };
    
    double r1 = func1( 0.5, 0.6 );
    cout << "a = " << a << endl << "b = " << b << endl
         << "c = " << c << endl << "d = " << d << endl 
         << "e = " << e << endl << "r1 = " << r1 << endl;
    
    auto func2 = [=,&c,&d,&e] () mutable {
        a++;
        b++;
        c++;  // 這幾個 call by ref
        d++;
        e++;
    };
    
    func2();
    cout << "a = " << a << endl << "b = " << b << endl
         << "c = " << c << endl << "d = " << d << endl 
         << "e = " << e << endl;
    
}





LambdaTestObj::LambdaTestObj()
    : a{1}, b{2}, c{3}, x{1.1}, y{2.2}, z{3.3} 
{
    cout << __func__ << endl;
}



void LambdaTestObj::test()
{
    int i = 10, j = 11;
    
    // 使用this, 可直接操作物件的成員變數
    auto func1 = [this] () -> int {
        a++; 
        b++;
        c++;
        double r = x + y + z;
        return static_cast<int>(r);
    };
    
    int r1 = func1();
    cout << "a = " << a << ", b = " << b << ", c = " << c << endl;
    cout << "r1 = " << r1 << endl;
    
    // 除了成員變數也可以操作其他local variable
    auto func2 = [this,&i,j] (char c) mutable  -> double {
        a++;
        b++;
        c++;
        i++;
        j++;
        cout << "c = " << c << endl;
        return x + y*z;
    };
    
    double r2 = func2('A');
    cout << "a = " << a << ", b = " << b << ", c = " << c << endl;
    cout << "i = " << i << ", j = " << j << endl;
    cout << "r2 = " << r2 << endl;
    
    // a, b, c 等成員變數 不能加入 capture, 必須用 this 的方式擷取
    auto func3 = [&i,&j] {
        i++;
        j++;
        cout << "i = " << i << ", j = " << j << endl;
    };
    
    func3();
    
}




void lambda_operator_6()
{
    // 可以遞迴呼叫, 不過實用性不高
    function<int(int)> func = [&] (int n) {
        if( n == 1 || n == 0 )
            return n;
        else
            return func(n-1) + func(n-2);
    };
    
    cout << func(5) << ", " << func(6) << ", " << func(7) << endl;
    
}





void lambda_operator_7()
{
    int a = 10, b = 100;

    // 這裡a, b的值會固定為 10, 100. 不會隨著底下 loop 改變.
    auto func = [=] ( int c ) -> int 
    {
        cout << "a = " << a << ", b = " << b << ", c = " << c << "\n";
        return a + b + c;
    };


    for( int i = 0; i < 10; i++ )
    {
        int sum = func( i );
        a += i;
        b += i;
        cout << "sum = " << sum << "\n";
    }

    printf("\n");
}







void lambda_operator_8()
{
    int a = 10, b = 100;

    // 這裡a, b的值會隨著 loop 而改變.
    auto func = [&] ( int c ) -> int 
    {
        cout << "a = " << a << ", b = " << b << ", c = " << c << "\n";
        return a + b + c;
    };


    for( int i = 0; i < 10; i++ )
    {
        int sum = func( i );
        a += i;
        b += i;
        cout << "sum = " << sum << "\n";
    }

    printf("\n");
}




void lambda_test_func()
{
    lambda_operator_1();
    lambda_operator_2();
    lambda_operator_3();
    lambda_operator_4();
    lambda_operator_5();
    lambda_operator_6();
    lambda_operator_7();
    lambda_operator_8();
}





} // end namespace c11
