#include "move_func.h"
#include <utility>


Move_1 C2_test_1( Move_1 c )
{
    //cout << "c addr = " << &c << endl;
    return Move_1( c.size + 10 );
}







Move_2 move_ct_test_1( Move_2 d )
{
    if( d.size > 0 )
        d.ptr[0]   =   1234;
    else
    {
        d.size = 1;
        d.ptr = new int[d.size];
    }

    return  d;
}





Move_2 move_ct_test_2( Move_2& d )
{
#if 1
    if( d.size > 0 )
        d.ptr[0]   =   9876;
    else
    {
        d.size = 1;
        d.ptr = new int[d.size];
    }
    
    return  d; // run copy constructor in D1 dd4 = move_ct_test_2(d2);
#else
    D1 tmp;
    tmp.size = d.size;
    tmp.ptr = new int[tmp.size];
    for( int i = 0; i < tmp.size; i++ )
        tmp.ptr[i] = d.ptr[i];
    
    return tmp; // run constructor in D1 dd4 = move_ct_test_2(d2);
#endif
}






Move_2&& move_ct_test_3( Move_2&& d )
{
    if( d.size > 0 )
        d.ptr[0]   =   -1234;
    else
    {
        d.size = 1;
        d.ptr = new int[d.size];
    }

    //return  d;  // compile fail.
    return std::move(d); // compile success.
                    //return std::forward<D1>(d); //compile success.
                    //return std::forward(d); // compile fail
}






Move_2 move_ct_test_4( int n )
{
    return Move_2(n + 10);
}







