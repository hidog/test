#include "cast_test.h"
#include <stdio.h>
#include <iostream>
#include "cast_obj.h"


using namespace std;




void static_cast_test()
{
    double a = 1234.15123;
    int b = static_cast<int>(a);
    cout << "b = " << b << "\n";
    
    CastBase *base = new CastDerive_1;
    static_cast<CastDerive_1 *>(base)->output_1(3);
    
    /*
        這邊會出現未知的執行結果
        static_cast無法判斷轉型是否正確.
    */
#if 0
    CastDerive_2* ptr = static_cast<CastDerive_2 *>(base);
    if( ptr != nullptr )
        ptr->output_2(5);
#endif

    delete base;
}





void dynamic_cast_test()
{
    CastBase *base = new CastDerive_1;
    dynamic_cast<CastDerive_1*>(base)->output_1(3);
    
    /*
        dynamic會轉出nullptr.
    */
    CastDerive_2* ptr = dynamic_cast<CastDerive_2 *>(base);
    if( ptr != nullptr )
        ptr->output_2(5);
    else
        std::cout << "ptr is null. convert fail.\n";
        
    delete base;
}




void reinterpret_cast_test()
{
    int64_t a = 0X0102030405060708;
    int n = sizeof(a);
    
    unsigned char *ptr = reinterpret_cast<unsigned char *>(&a);
    for( int i = 0; i < n; i++ )
        printf( "%d ", *(ptr + i) );
    std::cout << "\n";
}




void const_cast_test()
{
    const int a = 10;
    int& b = const_cast<int&>(a);
    b = 100;
    std::cout << "a = " << a << "\n";
    std::cout << "b = " << b << "\n"; // 意外的是, b = 100, a = 10.
    
    const int c = 10;
    int* d = const_cast<int*>(&c);
    *d = 100;
    std::cout << "c = " << c << "\n";
    std::cout << "d = " << *d << "\n";  // 結果同上
    
    int x = 10;
    const_sub(x);
    std::cout << "x = " << x << "\n";   // x = 100
    
    CastBase base;
    base.test();
    base.output();
    
}






void const_sub( const int& a )
{
    // a = 100; // it will cause build fail.
    int&b = const_cast<int&>(a);
    b = 100;
}



void cast_test_func()
{
    static_cast_test();
    dynamic_cast_test();
    reinterpret_cast_test();
    const_cast_test();
}
