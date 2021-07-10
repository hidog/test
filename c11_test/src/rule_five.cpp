#include "rule_five.h"
#include <iostream>

using namespace std;


namespace {



class Array
{
public:
    Array() : size{0}, data{nullptr} 
    {
        cout << "Array()" << endl;
    }
    
    Array( int _s ) : size{_s}
    {
        cout << "Array(int)" << endl;
        data = new int[_s];
        for( int i = 0; i < size; i++ )
            data[i] = i;
    }

    Array( const Array& _a ) : size{_a.size}, data{ new int [_a.size] }
    {
        cout << "copy constructor" << endl;
        for( int i = 0; i < size; i++ )
            data[i] = _a.data[i];
    }

    Array( Array&& _a ) noexcept : size{_a.size}, data{_a.data} 
    {
        _a.size = 0;
        _a.data = nullptr;
        cout << "move constructor" << endl;
    }

    Array& operator = ( const Array& _a )
    {
        cout << "copy operator =" << endl;
        size = _a.size;
        data = new int[size];
        for( int i = 0; i < size; i++ )
            data[i] = _a.data[i];
    }

    Array& operator = ( Array&& _a ) noexcept
    {
        cout << "move operator =" << endl;
        size = _a.size;
        data = _a.data;
        _a.size = 0;
        _a.data = nullptr;
    }

    ~Array()
    {
        delete [] data;
        data = nullptr;
        size = 0;
    }

    Array&& operator + ( const Array& _a )
    {
        Array tmp = *this;
        for( int i = 0; i < size; i++ )
            tmp.data[i] = data[i] + _a.data[i];
        return std::move(tmp);
    }

    void print()
    {
        for( int i = 0; i < size; i++ )
            cout << i << " " << data[i] << endl;
    }

private:
    int size = 0;
    int *data = nullptr;

};



} // end namespace




namespace c11 {


void test_rule_five()
{
    Array a(10);
    Array b{a};

    a.print();
    b.print();

    Array c = std::move(a + b);
    c.print();
}



} // end namespace c11