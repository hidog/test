#include "template_array.h"
#include <iostream>
#include <cassert>

using namespace std;




template<typename T>
TArray<T>::TArray() 
    : size{0}, data{nullptr} 
{
    cout << "TArray()" << endl;
}



template<typename T>
TArray<T>::TArray( int _s ) 
    : size{_s}
{
    cout << "TArray(int)" << endl;
    data = new T[_s];
    for( int i = 0; i < size; i++ )
        data[i] = static_cast<T>(i);
}



template<typename T>
TArray<T>::TArray( const TArray<T>& _a ) 
    : size{_a.size}, data{ new value_type [_a.size] }
{
    cout << "copy constructor" << endl;
    for( int i = 0; i < size; i++ )
        data[i] = _a.data[i];
}



template<typename T>
TArray<T>::TArray( TArray<T>&& _a ) noexcept 
    : size{_a.size}, data{_a.data} 
{
    _a.size = 0;
    _a.data = nullptr;
    cout << "move constructor" << endl;
}




template<typename T>
TArray<T>& TArray<T>::operator = ( const TArray& _a )
{
    cout << "copy operator =" << endl;
    size = _a.size;
    data = new T[size];
    for( int i = 0; i < size; i++ )
        data[i] = _a.data[i];

    return *this;
}



template<typename T>
TArray<T>& TArray<T>::operator = ( TArray&& _a ) noexcept
{
    cout << "move operator =" << endl;
    size = _a.size;
    data = _a.data;
    _a.size = 0;
    _a.data = nullptr;

    return *this;
}



template<typename T>
TArray<T>::~TArray()
{
    delete [] data;
    data = nullptr;
    size = 0;
}



template<typename T>
TArray<T>&& TArray<T>::operator + ( const TArray& _a )
{
    TArray<T> tmp = *this;
    for( int i = 0; i < size; i++ )
        tmp.data[i] = data[i] + _a.data[i];
    return std::move(tmp);

}




template<typename T>
T& TArray<T>::operator [] ( int index )
{
    if( index < size )
        return data[index];
    else
        return data[0]; // 使用 throw?
}





template<typename T>
T TArray<T>::operator () (int index)
{
    index = index < size ? index : size;
    T sum = 0;
    for( int i = 0; i < index; i++ )
        sum += data[i];
    return sum;
}





template<typename T>
void TArray<T>::print()
{
    for( int i = 0; i < size; i++ )
        cout << i << " " << data[i] << endl;
}





template<typename T>
T* TArray<T>::begin()
{
    return size > 0 ? &(data[0]) : nullptr;
}





template<typename T>
T* TArray<T>::end()
{
    return size > 0 ? &(data[size]) : nullptr;
}



// class 樣板具現化 Instantiate 
template class TArray<double>;
template class TArray<int>;


