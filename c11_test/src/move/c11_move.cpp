#include "c11_move.h"
#include <string>
#include <vector>
#include <iostream>

using namespace std;



namespace {





class C2
{
public:
    C2() : ptr(NULL), size(0)
    {
        //cout << "C2(), addr = " << this << endl;
        cout << "constructor C2()" << endl;
    }

    C2( int _s )
    {
        cout << "constructor C2(int)" << endl;
        //cout << "C2(int), addr = " << this << endl;
        size = _s;
        ptr = new int[size];
        for( int i = 0; i < size; i++ )
            ptr[i]  =   rand() % 10000;
    }

    C2( const C2& _c )
    {
        cout << "copy constructor" << endl;
        //cout << "C2(const C2&), addr = " << this << endl;
        size = _c.size;
        ptr = new int[size];
        for( int i = 0; i < size; i++ )
            ptr[i]  =   _c.ptr[i];
    }

    C2( C2&& _c )
    {
        cout << "move constructor\n";

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
        cout << "copy operator = , addr = " << this << " , _c addr = " << &_c << endl;

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
        cout << "C2 move operator = , addr = " << this << endl;

        ptr = _c.ptr;
        size = _c.size;

        _c.ptr = nullptr;
        _c.size = 0;

        return *this;
    }


    int size;
    int *ptr;
};




C2 C2_test_1( C2 c )
{
    //cout << "c addr = " << &c << endl;
    return C2( c.size + 10 );
}







class D1
{
public:
    D1() : ptr(NULL), size(0) { cout << "constructor" << endl; }

    D1( int _s )
    {
        //cout << "D1(int), addr = " << this << endl;
        // note: need check _s > 0
        cout << "constructor" << endl;
        size = _s;
        ptr = new int[size];
        for( int i = 0; i < size; i++ )
            ptr[i]  =   rand() % 10000;
    }

    D1( const D1& _d )
    {
        cout << "copy constructor D1(const D1&), addr = " << this << endl;
        size = _d.size;
        ptr = new int[size];
        for( int i = 0; i < size; i++ )
            ptr[i]  =   _d.ptr[i];
    }

    // move constructor
    D1( D1&& _d )
    {
        cout << "move constructor D1(D1&&), addr = " << this << endl;
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





class F
{
public:
    F() : v{1,2,3,4,5,6,7,8,9}
    {}

    F( int n )
    {
        v.resize(n);
        for( int i = 0; i < n; i++ )
            v[i] = rand() * 100;
    }

    F( const F& f )
    {
        v   =   f.v;
        v[0]++;
    }

    F( F&& f )
    {
        v   =   move(f.v);
        v[0] = -v[0];
    }

    ~F(){}

    vector<int> v;
};







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