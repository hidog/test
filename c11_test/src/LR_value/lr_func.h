#ifndef LR_FUNC_H
#define LR_FUNC_H

#include <iostream>





int& left_value();
int right_value();


void ref_test_1( int &x );
void ref_test_2( const int&x );
void ref_test_3( int&& x);
int ref_test_4();



void lr_test_f1( int&& a );


template<typename T>
void lr_test_f2( T&& a )
{
    std::cout << a << std::endl;
    a += 111;
    std::cout << a << std::endl;
    // a 能取址,變成一個左值
    std::cout << "addr a = " << &a << std::endl;
}






#endif