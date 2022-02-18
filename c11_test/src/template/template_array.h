#ifndef TEMPLATE_ARRAY_H
#define TEMPLATE_ARRAY_H



template<typename T>
class TArray
{
public:
    using value_type = T;

    TArray();
    TArray( int _s );

    TArray( const TArray& _a );
    TArray( TArray&& _a ) noexcept;

    TArray& operator = ( const TArray& _a );
    TArray& operator = ( TArray&& _a ) noexcept;
    ~TArray();

    TArray&& operator + ( const TArray& _a );
    value_type& operator [] ( int index );

    void print();

    value_type* begin();
    value_type* end();

    value_type operator () (int index);

private:
    int size = 0;
    value_type *data = nullptr;

};



#endif