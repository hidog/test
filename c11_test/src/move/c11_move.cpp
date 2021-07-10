#include "c11_move.h"
#include <string>
#include <vector>
#include <iostream>

#include "test_class_2.h"
#include "test_class_3.h"
#include "test_class_4.h"


using namespace std;



namespace {






C2 C2_test_1( C2 c )
{
    //cout << "c addr = " << &c << endl;
    return C2( c.size + 10 );
}







D1 move_ct_test_1( D1 d )
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


D1 move_ct_test_2( D1& d )
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



D1&& move_ct_test_3( D1&& d )
{
    if( d.size > 0 )
        d.ptr[0]   =   -1234;
    else
    {
        d.size = 1;
        d.ptr = new int[d.size];
    }

    //return  d;  // compile fail.
    return move(d); // compile success.
                    //return std::forward<D1>(d); //compile success.
                    //return std::forward(d); // compile fail
}

D1 move_ct_test_4( int n )
{
    return D1(n + 10);
}









} // end namespace









namespace c11 {




void move_test_1()
{
    string str = "hello";
    vector<string> vec;

    vec.push_back(str);
    cout << vec[0] << " " << str << endl;

    vec.push_back( move(str) );
    cout << vec[1] << " " << str << endl;
}






void move_test_2()
{
    //C2 &c2 = C2(1000);
    C2 c2a = C2(1000);
    C2 c2b = std::move( C2(2000) );
    cout << c2b.ptr[0] << endl;
    //cout << "c2a addr = " << &c2a << endl;

    C2 c2c = C2_test_1( c2a ); // run constructor
    C2 c2d = std::move( C2_test_1(c2b) );
    //cout << "c2b addr = " << &c2b << endl;
    c2c = C2_test_1( c2a );
    cout << c2c.ptr[0] << endl;

    c2c = c2a; // call copy assignment =
    cout << c2c.ptr[0] << endl;

    c2c = C2(321);  // call move assignment.
    cout << c2c.size << endl;

    /*
    if we don't define move assign, it will call assign.
    if we define assign as C2& operator = ( C2& _c ) without const, it will compile error.
    */
}







// note: Ä~©ÓÃö«Y°Ñ¦Ò http://en.cppreference.com/w/cpp/language/move_assignment
void move_test_3()
{
    D1 d1(1000), d2(1500);

    //D1 d3( d1 + d2 );
    D1 d3( std::move(d1+d2) );
    cout << d1.ptr[0] << " " << d2.ptr[0] << " " << d3.ptr[0] << endl;

    /*
    this code will not run into move constructor.
    I gauss the reason is RVO.
    */
    D1 d4( move(d1+d2) );
    cout << d4.ptr[0] << endl;

    D1 dd1 = std::move(d1);
    cout << d1.size << " " << dd1.size << endl;

    D1 dd2 = move_ct_test_1(d2);
    cout << dd2.ptr[0] << endl;

    D1 dd3 = move_ct_test_1( D1(234) );
    cout << dd3.size << endl;

    D1 dd4 = move_ct_test_2(d2); // see comment move_ct_test_2
    cout << dd4.ptr[0] << endl;

    //D1 dd5 = move_ct_test_2( D(111) ); // compile fail.

    //D1 dd6 = move_ct_test_3( d2 ); // compile fail.
    D1 dd7 = move_ct_test_3( D1(323) );
    cout << dd7.ptr[0] << endl;

    D1 dd8 = move_ct_test_4( 1000 );
    cout << dd8.ptr[0] << endl;
}






void move_test_4()
{
    F f1;
    cout << f1.v[0] << endl;

    F f2(f1);
    cout << f2.v[0] << endl;

    F f3(move(f1));
    cout << f3.v[0] << " " << f1.v.size() << endl;

    F f4 = move(F{100});    // note: if we don't use std::move, it will not call move constructor. (RVO)
    cout << f4.v[0] << " " << f4.v.size() << endl;
}





} // end namespace c11