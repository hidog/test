#include "test.h"
#include <iostream>


Test::Test()
{
    int i;
    data    =   new int[100];
    for( i = 0; i < 100; i++ )
        data[i]    =   i;
}


Test::~Test()
{
    delete [] data;
    data = nullptr;
}


void Test::print()
{
    int i;
    for( i = 0; i < 100; i++ )
        std::cout << data[i] << " ";
    std::cout << "\n";
}
