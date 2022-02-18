#include "move_class_3.h"
#include <cstdlib>


Move_3::Move_3() 
    : v{1,2,3,4,5,6,7,8,9}
{}




Move_3::Move_3( int n )
{
    v.resize(n);
    for( int i = 0; i < n; i++ )
        v[i] = rand() * 100;
}





Move_3::Move_3( const Move_3& f )
{
    v   =   f.v;
    v[0]++;
}




Move_3::Move_3( Move_3&& f )
{
    v   =   move(f.v);
    v[0] = -v[0];
}




Move_3::~Move_3()
{}



