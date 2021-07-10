#include "rule_three.h"
#include <iostream>

using namespace std;



namespace {

class C1
{
public:
    C1() : ptr(NULL), size(0)
    {
        cout << "C1(), addr = " << this << endl;
    }

    C1( int _s )
    {
        cout << "C1(int), addr = " << this << endl;
        size = _s;
        ptr = new int[size];
        for( int i = 0; i < size; i++ )
            ptr[i]  =   rand() % 10000;
    }

    C1( const C1& _c )
        : C1() // c++11 only
    {
        cout << "C1(const C1&), addr = " << this << endl;
        size = _c.size;
        ptr = new int[size];
        for( int i = 0; i < size; i++ )
            ptr[i]  =   _c.ptr[i];
    }

    ~C1()
    {
        cout << "~C1(), addr = " << this << endl;
        delete [] ptr;
    }

    C1& operator = ( const C1& _c )
    {
        cout << "C1 operator = , addr = " << this << " , _c addr = " << &_c << endl;

        if( size != 0 && ptr != NULL )
            delete [] ptr;

        size = _c.size;
        ptr = new int[size];
        for( int i = 0; i < size; i++ )
            ptr[i] = _c.ptr[i];

        return *this;
    }

    int size;
    int *ptr;
};




C1 C1_test_1( C1 c )
{
    cout << "c addr = " << &c << endl;
    return C1( c.size + 10 );

    /*
    note: Return value optimization (RVO),
    result is difference from 
    C1 tmp;
    ...
    return tmp;
    */
}







} // end namespace







// traditional rule of three.
// 姑且不論寫法上有一些瑕疵
void test_rule_three()
{
    C1 c1(10000);
    cout << c1.ptr[0] << endl;
    cout << "c1 addr = " << &c1 << endl;

    C1 c2;
    cout << "c2 addr = " << &c2 << endl;
    c2 = C1_test_1( c1 );
    cout << c2.ptr[0] << endl;
}


