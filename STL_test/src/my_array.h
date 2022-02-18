#ifndef MY_ARRAY_H
#define MY_ARRAY_H


#include <string>



class MyArray
{
public :
    MyArray();
    ~MyArray();
    
    MyArray( std::string str1, std::string str2 );
    
    MyArray( const MyArray& m );
    MyArray( MyArray&& m );
    
    MyArray& operator = ( const MyArray& m );
    MyArray& operator = ( MyArray&& m );
    
    void print();

    int *data   =   nullptr;
    static constexpr int DATA_SIZE = 5;
};


MyArray operator + ( const MyArray& m1, const MyArray& m2 );



void reset_shift();


#endif // TEST_H
