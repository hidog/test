#ifndef PF_FUNC_H
#define PF_FUNC_H

#include <memory>
#include "pf_class.h"



// U derive by u.
template<typename T, typename U>
std::unique_ptr<T> pf_test_1( U&& u )
{
    return std::unique_ptr<T>( new T(u) );
}





// U derive by u.
template<typename T, typename U>
std::unique_ptr<T> pf_test_2( U&& u )
//std::unique_ptr<T> pf_test_2( U u ) // cause both rvalue
{
    // 不加 forward, 這邊會變成copy constructor, 多複製一次資料造成效能變差
    // 加上 forward, 用move constructor, 如果右值傳入能保持右值特性, 進去move constructor, 改善效能.
    return std::unique_ptr<T>( new T( std::forward<U>(u) ) );
    //return std::unique_ptr<T>( new T(u) );  // cause all run lvalue.   ( u = 3; T(u); u is lvalue. )
}






template<typename T, typename... U>
std::unique_ptr<T> pf_test_3( U&&... u )
{
    return std::unique_ptr<T>( new T(std::forward<U>(u)...) );
}





std::unique_ptr<PF_A> pf_test_4( int&& u );
std::unique_ptr<PF_A> pf_test_5( const int& u );
void pf_test_6( int && a );





template<typename T>
void pf_test_7( T&& a )
{
#if 0
    if( a == 0 )
    {
        std::cout << "\n";
        return;
    }
    else
    {
        std::cout << a << ", ";
        --a;
        pf_test_7(a);     // 編譯通過 (因為改成樣板)
        pf_test_7(a-1);   // 編譯通過
        // 不加forward的情況下, 無法保持左值或右值
    }
#elif 1
    if( a == 0 )
    {
        std::cout << std::endl;
        return;
    }
    else
    {
        std::cout << a << ", ";
        --a;
        pf_test_6( std::forward<int>(a) );  // 這邊編譯通過. 加上forward能保持左值或右值性質. (perfect forward)
    }
#endif
}




void pf_test_8( PF1&& pf );
void pf_test_9( PF1&& pf );


template<typename T>
void pf_test_10( T&& pf )
{
    pf.output();
    pf_test_11( std::forward<T>(pf) );
}


template<typename T>
void pf_test_11( T&& pf )
{
    pf.output();
}




#endif