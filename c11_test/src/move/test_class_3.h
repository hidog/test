#ifndef TEST_CLASS_3_H
#define TEST_CLASS_3_H

#include <iostream>


class D1
{
public:
    D1() : ptr(NULL), size(0) { std::cout << "constructor" << std::endl; }

    D1( int _s )
    {
        //cout << "D1(int), addr = " << this << endl;
        // note: need check _s > 0
        std::cout << "constructor" << std::endl;
        size = _s;
        ptr = new int[size];
        for( int i = 0; i < size; i++ )
            ptr[i]  =   rand() % 10000;
    }

    D1( const D1& _d )
    {
        std::cout << "copy constructor D1(const D1&), addr = " << this << std::endl;
        size = _d.size;
        ptr = new int[size];
        for( int i = 0; i < size; i++ )
            ptr[i]  =   _d.ptr[i];
    }

    // move constructor
    D1( D1&& _d )
    {
        std::cout << "move constructor D1(D1&&), addr = " << this << std::endl;
        size = _d.size;
        ptr = _d.ptr;

        _d.ptr = nullptr;
        _d.size = 0;
    }

    ~D1()
    {
        //cout << "~D1(), addr = " << this << endl;
        delete [] ptr;
    }


    D1 operator + ( const D1& _d )
    {
        D1 tmp( _d.size + size );
        //return D1( _d.size + size );
        return tmp;
    }


    int size;
    int *ptr;
};


#endif