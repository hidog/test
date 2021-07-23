#include "rule_five.h"
#include <iostream>


using namespace std;
static int cc = 0;

namespace {



class Array
{
public:
    
    Array() : size{0}, data{nullptr}, index{cc++}
    {
        cout << "Array()" << endl;
    }
    
    Array( int _s ) : size{_s}, index{cc++}
    {
        cout << "Array(int)" << endl;
        data = new int[_s];
        for( int i = 0; i < size; i++ )
            data[i] = i;
    }

    Array( const Array& _a ) : size{_a.size}, data{ new int [_a.size] }, index{cc++}
    {
        cout << "copy constructor" << endl;
        for( int i = 0; i < size; i++ )
            data[i] = _a.data[i];
    }

    Array( Array&& _a ) 
    {
        cout << _a.index << endl;
        
        size = _a.size;
        data = _a.data;
        
        _a.size = 0;
        _a.data = nullptr;
        cout << "move constructor" << endl;
    }

    Array& operator = ( const Array& _a )
    {
        cout << "copy operator =" << endl;
        size = _a.size;
        index = _a.index;
        data = new int[size];
        for( int i = 0; i < size; i++ )
            data[i] = _a.data[i];
        return *this;
    }

    Array& operator = ( Array&& _a ) noexcept
    {
        cout << "move operator =" << endl;
        size = _a.size;
        data = _a.data;
        index = _a.index;
        _a.size = 0;
        _a.data = nullptr;
        return *this;
    }

    ~Array()
    {
        cout << index << endl;
        delete [] data;
        data = nullptr;
        size = 0;
    }

    Array operator + ( const Array& _a )
    {
        Array tmp{_a.size};
        for( int i = 0; i < size; i++ )
            tmp.data[i] = data[i] + _a.data[i];
        return tmp;
    }

    void print()
    {
        for( int i = 0; i < size; i++ )
            cout << i << " " << data[i] << endl;
    }

    friend Array add_5_func( Array a );

private:
    int size = 0;
    int *data = nullptr;
    int index;
};


Array add_5_func( Array a )
{
    for( int i = 0; i < a.size; i++ )
        a.data[i] += 5;
    return a;
}



} // end namespace




namespace c11 {


void test_rule_five_1()
{
    Array a(10);
    Array b{a};

    a.print();
    b.print();

    Array c;
    c = a + b;
    c.print();
}



void test_rule_five_2()
{
    Array a(10);
    Array d = add_5_func(a);
    d.print();
}




} // end namespace c11
