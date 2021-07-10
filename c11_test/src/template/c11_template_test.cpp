#include "c11_template_test.h"

#include <iostream>
#include "template_func.h"

using namespace std;


namespace {

template<typename T>
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
        data = new T[_s];
        for( int i = 0; i < size; i++ )
            data[i] = static_cast<T>(i);
    }

    Array( const Array& _a ) : size{_a.size}, data{ new T [_a.size] }
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
        data = new T[size];
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

    T& operator [] ( int index )
    {
        if( index < size )
            return data[index];
        else 
            return nullptr;
    }

    void print();

    T* begin();
    T* end();


    T operator () (int index)
    {
        index = index < size ? index : size;
        T sum = 0;
        for( int i = 0; i < index; i++ )
            sum += data[i];
        return sum;
    }


private:
    int size = 0;
    T *data = nullptr;

};


template<typename T>
void Array<T>::print()
{
    for( int i = 0; i < size; i++ )
        cout << i << " " << data[i] << endl;
}


template<typename T>
T* Array<T>::begin()
{
    return size > 0 ? &(data[0]) : nullptr;
}


template<typename T>
T* Array<T>::end()
{
    return size > 0 ? &(data[size]) : nullptr;
}


} // end namespace








namespace c11{



void test_template()
{
    Array<double> a{10};
    a.print();
    for( auto &itr : a )
        cout << itr << " ";
}



void test_func_obj()
{
    Array<int> a_func(5);
    cout << a_func(3); // a_fun·Q¹³¦¨function
}


void test_variadic_template()
{
    cout << sum( 1, 1.2 );
    v_func( 1, 3, 4.4, "abcde", 'Z' );
}


} // end namespace c11