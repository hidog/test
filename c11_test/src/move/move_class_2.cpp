#include "move_class_2.h"
#include <iostream>






Move_2::Move_2() 
    : ptr(NULL), size(0) 
{ 
    std::cout << "constructor" << std::endl; 
}





Move_2::Move_2( int _s )
{
    //cout << "D1(int), addr = " << this << endl;
    // note: need check _s > 0
    std::cout << "constructor" << std::endl;
    size = _s;
    ptr = new int[size];
    for( int i = 0; i < size; i++ )
        ptr[i]  =   rand() % 10000;
}





Move_2::Move_2( const Move_2& _d )
{
    std::cout << "copy constructor D1(const D1&), addr = " << this << std::endl;
    size = _d.size;
    ptr = new int[size];
    for( int i = 0; i < size; i++ )
        ptr[i]  =   _d.ptr[i];
}





// move constructor
Move_2::Move_2( Move_2&& _d ) noexcept
{
    std::cout << "move constructor D1(D1&&), addr = " << this << std::endl;
    size = _d.size;
    ptr = _d.ptr;

    _d.ptr = nullptr;
    _d.size = 0;
}





Move_2::~Move_2()
{
    //cout << "~D1(), addr = " << this << endl;
    delete [] ptr;
}






Move_2 Move_2::operator + ( const Move_2& _d )
{
    Move_2 tmp( _d.size + size );
    //return D1( _d.size + size );
    return tmp;
}


