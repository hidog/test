#include "cast_obj.h"

#include <iostream>


void CastBase::output()
{
    std::cout << a << ", " << b << "\n";
}



void CastDerive_1::output()
{
    std::cout << c << ", " << d << "\n";
    CastBase::output();
}



void CastDerive_1::output_1( int n )
{
    for( int i = 0; i < n; i++ )
        std::cout << c;
    std::cout << "\n";
}



void CastDerive_2::output_2( int n )
{
    for( int i = start; i <= end; i++ )
        std::cout << "i = " << i << ", i*n = " << i*n << "\n";
}



void CastDerive_2::output()
{
    for( int i = start; i <= end; i++ )
        std::cout << i << " ";
    std::cout << "\n";
}


void CastBase::test() const
{
    const_cast<CastBase *>(this)->a = 9999;
    // a = 991; // it will cause compile fail.
}
