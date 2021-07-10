#ifndef TEST_CLASS_2_H
#define TEST_CLASS_2_H

#include <iostream>


class C2
{
public:
    C2() : ptr(NULL), size(0)
    {
        //cout << "C2(), addr = " << this << endl;
        std::cout << "constructor C2()" << std::endl;
    }

    C2( int _s )
    {
        std::cout << "constructor C2(int)" << std::endl;
        //cout << "C2(int), addr = " << this << endl;
        size = _s;
        ptr = new int[size];
        for( int i = 0; i < size; i++ )
            ptr[i]  =   rand() % 10000;
    }

    C2( const C2& _c )
    {
        std::cout << "copy constructor" << std::endl;
        //cout << "C2(const C2&), addr = " << this << endl;
        size = _c.size;
        ptr = new int[size];
        for( int i = 0; i < size; i++ )
            ptr[i]  =   _c.ptr[i];
    }

    C2( C2&& _c )
    {
        std::cout << "move constructor\n";

        ptr     =   _c.ptr;
        size    =   _c.size;

        _c.ptr  =   nullptr;
        _c.size =   0;
    }


    ~C2()
    {
        //cout << "~C2(), addr = " << this << endl;
        delete [] ptr;
    }

    C2& operator = ( const C2& _c )
        //C2& operator = ( C2& _c )
    {
        std::cout << "copy operator = , addr = " << this << " , _c addr = " << &_c << std::endl;

        if( size != 0 && ptr != NULL )
            delete [] ptr;

        size = _c.size;
        ptr = new int[size];
        for( int i = 0; i < size; i++ )
            ptr[i] = _c.ptr[i];

        return *this;
    }


    // move assignment
    C2& operator = ( C2&& _c )
    {
        std::cout << "C2 move operator = , addr = " << this << std::endl;

        ptr = _c.ptr;
        size = _c.size;

        _c.ptr = nullptr;
        _c.size = 0;

        return *this;
    }


    int size;
    int *ptr;
};





#endif