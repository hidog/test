#include "array.h"
#include <iostream>
#include <stdlib.h>

using namespace std;




Array::Array() 
    : ptr(NULL), size(0)
{
    cout << "Array(), addr = " << this << endl;
}




Array::Array( int _s )
{
    cout << "Array(int), addr = " << this << endl;
    size = _s;
    ptr = new int[size];
    for( int i = 0; i < size; i++ )
        ptr[i]  =   rand() % 100;
}





Array::Array( const Array& _a )
{
    cout << "Array(const Array&), addr = " << this << endl;
    size = _a.size;
    ptr = new int[size];
    for( int i = 0; i < size; i++ )
        ptr[i]  =   _a.ptr[i];
}





Array::~Array()
{
    cout << "~Array(), addr = " << this << endl;
    delete [] ptr;    
}





Array& Array::operator = ( const Array& _a )
{
    if( &_a == this )
    {
        cout << "self assign." << endl;
        return *this;
    }

    cout << "Array operator = , addr = " << this << " , _a addr = " << &_a << endl;

    if( size != 0 && ptr != NULL )
    {
        delete [] ptr;
        size = 0;
    }

    size = _a.size;
    ptr = new int[size];
    for( int i = 0; i < size; i++ )
        ptr[i] = _a.ptr[i];

    return *this;
}





void Array::output()
{
    cout << "Array::output\n";
    cout << "size = " << size << endl;
    for( int i = 0; i < size; i++ )
        cout << ptr[i] << ", ";
    cout << endl;
}





void Array::add( int n )
{
    for( int i = 0; i < size; i++ )
        ptr[i]  +=  n;
}



void Array::output2()
{
    cout << "Array::output2\n";
    cout << "size = " << size << endl;
    for( int i = 0; i < size; i++ )
        cout << ptr[i] << ", ";
    cout << endl;
}
