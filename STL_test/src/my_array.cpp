#include "my_array.h"
#include <iostream>

using namespace std;


namespace {

int shift_start =   0;

} // end namespace




void reset_shift()
{
    shift_start =   0;
}





MyArray operator + ( const MyArray& m1, const MyArray& m2 )
{
    MyArray m;
    for( int i = 0; i < MyArray::DATA_SIZE; i++ )
        m.data[i]   =   m1.data[i] + m2.data[i];
    return m;
}




MyArray::MyArray()
{
    int i;
    data    =   new int[DATA_SIZE];
    for( i = 0; i < DATA_SIZE; i++ )
        data[i]    =   i + shift_start;
    
    shift_start++;
    cout << "MyArray::MyArray\n";
}




MyArray::~MyArray()
{
    delete [] data;
    data = nullptr;
    
    cout << "MyArray::~MyArray\n";
}




void MyArray::print()
{
    if( data == nullptr )
        cout << "data has moved.\n";
    else
    {
        int i;
        for( i = 0; i < DATA_SIZE; i++ )
            std::cout << data[i] << " ";
        cout << "\n";
    }
}




MyArray::MyArray( const MyArray& m )
{
    int i;
    
    data    =   new int[DATA_SIZE];
    for( i = 0; i < DATA_SIZE; i++ )
        data[i] =   m.data[i];
    
    cout << "MyArray::MyArray( const MyArray& m )\n";
}





MyArray::MyArray( MyArray&& m )
{
    data    =   m.data;
    m.data  =   nullptr;
    cout << "MyArray::MyArray( MyArray&& m )\n";
}




MyArray& MyArray::operator = ( const MyArray& m )
{
    int i;
    
    data    =   new int[DATA_SIZE];
    for( i = 0; i < DATA_SIZE; i++ )
        data[i] =   m.data[i];
    
    cout << "MyArray& MyArray::operator = ( const MyArray& m )\n";
    return *this;
}


    

MyArray& MyArray::operator = ( MyArray&& m )
{
    data    =   m.data;
    m.data  =   nullptr;
    cout << "MyArray& MyArray::operator = ( MyArray&& m )\n";
    return  *this;
}



MyArray::MyArray( std::string str1, std::string str2 )
{
    cout << str1 << ", " << str2 << endl;
    
    int i;
    data    =   new int[DATA_SIZE];
    for( i = 0; i < DATA_SIZE; i++ )
        data[i]    =   i + shift_start;
    
    shift_start++;
    cout << "MyArray::MyArray\n";
}
