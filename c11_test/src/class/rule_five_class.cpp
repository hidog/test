#include "rule_five_class.h"
#include <iostream>
#include <string>

using namespace std;



Array::Array()
    : size{0}, data{nullptr}
{
    cout << "Array::Array()" << endl;
}



Array::Array( int _s ) 
    : size{_s}
{
    cout << "Array::Array( int _s ) " << endl;
    data = new int[size];
    for( int i = 0; i < size; i++ )
        data[i] = i;
}



Array::Array( const Array& _a ) 
    : size{_a.size}, data{ new int [_a.size] }
{
    cout << "Array::Array( const Array& _a )" << endl;
    for( int i = 0; i < size; i++ )
        data[i] = _a.data[i];
}




Array::Array( Array&& _a ) noexcept
{
    cout << "Array::Array( Array&& _a ) noexcept" << endl;

#if 0
    size = _a.size;
    data = _a.data;

    _a.size = 0;
    _a.data = nullptr;
#else
    size = 0;
    data = nullptr;

    std::swap( size, _a.size );
    std::swap( data, _a.data );
#endif
}



Array& Array::operator = ( const Array& _a )
{
    cout << "Array& Array::operator = ( const Array& _a )" << endl;

    delete [] data;

    size = _a.size;
    data = new int[size];

    for( int i = 0; i < size; i++ )
        data[i] = _a.data[i];

    return *this;
}



Array& Array::operator = ( Array&& _a ) noexcept
{
    cout << "Array& Array::operator = ( Array&& _a ) noexcept" << endl;

#if 0
    delete [] data;

    size = _a.size;
    data = _a.data;

    _a.size = 0;
    _a.data = nullptr;
#else
    std::swap( size, _a.size );
    std::swap( data, _a.data );
#endif

    return *this;
}



Array::~Array()
{
    cout << __FUNCTION__ << endl;
    delete [] data;
    size = 0;
}





Array operator + ( const Array& a, const Array& b )
{
    int min_size = a.size < b.size ? a.size : b.size;
    Array tmp{min_size};

    for( int i = 0; i < min_size; i++ )
        tmp[i] = a[i] + b[i];

    return tmp;
}




void Array::print()
{
    cout << endl;
    for( int i = 0; i < size; i++ )
        cout << "data[" << i << "] = " << data[i] << ", ";
    cout << endl << endl;
}





Array add_5_func( Array a )
{
    for( int i = 0; i < a.size; i++ )
        a.data[i] += 5;
    return a;
}




Array add_10_func( Array&& a )
{
    for( int i = 0; i < a.size; i++ )
        a.data[i] += 5;
    return a; // note: xcode環境下這邊會跳warning. 
}





int& Array::operator [] ( int index )
{
    if( index < size && index >= 0 )
        return data[index];
    else
    {
        //cout << "error\n";
        //return data[0];
        std::string message = "index out of range. index = " + std::to_string(index) + 
                              ", size = " + std::to_string(size);
        throw std::out_of_range(message);
    }
}



const int& Array::operator [] ( int index ) const
{
    if( index < size )
        return data[index];
    else
    {
        //cout << "error\n";
        //return data[0];
        std::string message = "index out of range. index = " + std::to_string(index) + 
                              ", size = " + std::to_string(size);
        throw std::out_of_range(message);
    }
}



int* Array::begin()
{
    return data;
}


int* Array::end()
{
    return data + size;
}
