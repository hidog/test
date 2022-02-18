#ifndef ARRAY_H
#define ARRAY_H




class Array
{
public:
    Array();
    Array( int _s );
    Array( const Array& _a );
    ~Array();  // 不加virtual, 會造成多型的時候無法執行到基底的destructor.

    Array& operator = ( const Array& _c );

    void output();
    void add( int n );
    
    virtual void output2();

private:
    int size;
    int *ptr;
};



#endif
