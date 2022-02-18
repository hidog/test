#ifndef RULE_FIVE_CLASS_H
#define RULE_FIVE_CLASS_H

#include <memory>
#include <utility>


class Array
{
public:
    Array();
    Array( int _s );
    Array( const Array& _a );
    Array( Array&& _a ) noexcept;
    ~Array();

    Array& operator = ( const Array& _a );
    Array& operator = ( Array&& _a ) noexcept;

    void print();
    //friend Array add_5_func( Array a );

    int& operator [] ( int index );
    const int& operator [] ( int index ) const;

    int size;
    int *data;
    
    int* begin(); // 宣告begin, end, 才能使用 range-for
    int* end();
};


Array operator + ( const Array& a, const Array& b );



Array add_5_func( Array a );
Array add_10_func( Array&& a );




template<typename T, typename U>
std::unique_ptr<T> make_instance( U&& a )
{
    return std::unique_ptr<T>( new T{ std::forward<U>(a) } );
}



template<typename T, typename U>
std::unique_ptr<Array> make_instance_2( U&& a )
{
    return std::unique_ptr<T>( new T{a} );
}






#endif
