#ifndef INITIAL_OBJ_H
#define INITIAL_OBJ_H

#include <initializer_list>
#include "initial_base.h"


class InitialObj
{
public:
    InitialObj();
    InitialObj( int _d1, int _d2 );
    InitialObj( const InitialObj& aa );

    // initializer list
    InitialObj( std::initializer_list<int> list );

    // it will run initializer_list<int>, not run initializer_list<short>
    InitialObj( std::initializer_list<short> list );

    ~InitialObj();

    int data1 = 11;
    int data2{99};
    int *ptr = nullptr;

    //B b(99); // compile fail.
    InitialBase b1{99};
    InitialBase b2 = {99};
    InitialBase b3 = {-4, 4.5442};

};

#endif