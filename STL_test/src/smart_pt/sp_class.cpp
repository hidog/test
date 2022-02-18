#include "sp_class.h"

#include <iostream>
#include <cstdlib>
#include <string.h>



using namespace std;




SpFoo::SpFoo()
    : age{0}, height{new double{0}}, array{new short[10]{0}}
{
    cout << "SpFoo constructor\n";
}





SpFoo::SpFoo( int a, double h, int value )
    : age{a}, height{new double{h}}
{
    array   =   new short[size];
    for( int i = 0; i < size; i++ )
        array[i] = value;
    
    cout << "SpFoo constructor with input\n";
}






SpFoo::~SpFoo()
{
    delete [] array;
    delete height;
    
    cout << "SpFoo destructor\n";
}






SpFoo::SpFoo( const SpFoo& foo )
    : SpFoo( foo.age, *foo.height )
{
    // note : array alloc mem in delegation constructor.
    delete [] array;
    array = new short[size];
    for( int i = 0; i < size; i++ )
        array[i] = foo.array[i];
    
    cout << "SpFoo copy constructor\n";
}





SpFoo& SpFoo::operator = ( const SpFoo& foo )
{
    age      =   foo.age;
    *height  =   *foo.height;
    
    delete [] array;
    array = new short[size];
    for( int i = 0; i < size; i++ )
        array[i] = foo.array[i];
    
    cout << "SpFoo copy assignment\n";
    
    return  *this;
}






SpFoo::SpFoo( SpFoo&& foo )
{
    age     =   foo.age;
    
    delete height;
    height  =   foo.height;
    
    delete [] array;
    array   =   foo.array;
    
    foo.height  =   nullptr;
    foo.array   =   nullptr;
    
    cout << "SpFoo move constructor\n";
}





SpFoo& SpFoo::operator = ( SpFoo&& foo )
{
    age     =   foo.age;
    
    delete height;
    height  =   foo.height;
    
    delete [] array;
    array   =   foo.array;
    
    foo.height  =   nullptr;
    foo.array   =   nullptr;
    
    cout << "SpFoo move assignment\n";
    
    return *this;
}







void SpFoo::output()
{
    cout << "age = " << age << endl
         << "height = " << *height << endl;
    
    cout << "array = ";
    for( int i = 0; i < size; i++ )
        cout << array[i] << " ";
    cout << endl;
}





void SpFoo::shuffle_array( int lower, int upper )
{
    int range   =   upper - lower;
    if( range <= 0 )
        range = 10;
    
    for( int i = 0; i < size; i++ )
        array[i] = rand() % range + lower;
}








SpBar::SpBar()
    // : name{new char[10]{"Default"}}, name_len{10}, SpFoo{}  ubuntu環境下 編譯錯誤
    : SpFoo{}
{
    name = new char[10];
    name_len = 10;
    strcpy( name, "Default" );

    cout << "SpBar constructor\n";
}





SpBar::SpBar( int a, double h, const char* const n )
    : SpFoo( a, h )
{
    name_len    =   (int)strlen(n) + 1;
    name        =   new char[name_len];
    strcpy( name, n );
    
    cout << "SpBar constructor with input\n"; 
}





SpBar::~SpBar()
{
    delete [] name; name = nullptr;
    cout << "SpBar destructor\n";
}





SpBar operator + ( const SpBar& a, const SpBar& b )
{
    SpBar c;
    
    // foo
    c.age       =   a.age + b.age;
    *c.height   =   *a.height + *b.height;
    
    // 固定size, 這邊可以省略 c.array的mem控制
    for( int i = 0; i < SpFoo::size; i++ )
        c.array[i]  =   a.array[i] + b.array[i];
    
    // bar
    c.name_len  =   a.name_len + b.name_len;
    delete [] c.name;
    c.name = new char[c.name_len];
    
    int count = 0;
    
    for( int i = 0; i < a.name_len; i++ )
    {
        if( a.name[i] != '\0' )
            c.name[count++] = a.name[i];
    }
    
    for( int i = 0; i < b.name_len; i++ )
    {
        if( b.name[i] != '\0' )
            c.name[count++] = b.name[i];
    }
    
    c.name[count] = '\0';
    
    return c;
}




SpBar::SpBar( const SpBar& bar )
    : SpFoo(bar)
{
    name_len = bar.name_len;
    name = new char[name_len];
    strcpy( name, bar.name );
    
    cout << "SpBar copy constructor\n";
}








SpBar& SpBar::operator = ( const SpBar& bar )
{
    //*this = bar; // 跳warning. 會變遞迴呼叫
    dynamic_cast<SpFoo&>(*this) = bar;
    //dynamic_cast<SpFoo&>(*this) = dynamic_cast<const SpFoo&>(bar);
    
    delete [] name;
    name_len = bar.name_len;
    name = new char[name_len];
    strcpy( name, bar.name );
    
    cout << "SpBar copy assignment\n";
    
    return *this;
}





SpBar::SpBar( SpBar&& bar )
    //: SpFoo( bar )  // 會變copy constructor
    : SpFoo( std::forward<SpBar>(bar) )
{
    name_len = bar.name_len;
    name = bar.name;
    
    bar.name_len = 0;
    bar.name = nullptr;
    
    cout << "SpBar move constructor\n";
}




SpBar& SpBar::operator = ( SpBar&& bar )
{
    //dynamic_cast<SpFoo&>(*this) = bar; // 會變copy assignment
    dynamic_cast<SpFoo&>(*this) = std::forward<SpBar>(bar);
    
    name_len = bar.name_len;
    name = bar.name;
    
    bar.name_len = 0;
    bar.name = nullptr;
    
    cout << "SpBar move assignment\n";
    
    return *this;
}





void SpBar::output()
{
    SpFoo::output();
    cout << "name = " << name << endl;
}











SpQux::SpQux()
    : abbr('*'), SpFoo{}
{
    cout << "SpQux constructor\n";
}





SpQux::SpQux( int a, double h, char c )
    : SpFoo( a, h ), abbr(c)
{
    cout << "SpQux constructor with input\n"; 
}





SpQux::~SpQux()
{
    cout << "SpQux destructor\n";
}






SpQux::SpQux( const SpQux& qux )
    : SpFoo(qux)
{
    abbr = qux.abbr;
    cout << "SpQux copy constructor\n";
}








SpQux& SpQux::operator = ( const SpQux& qux )
{
    dynamic_cast<SpFoo&>(*this) = qux;
    abbr = qux.abbr;
    cout << "SpQux copy assignment\n";
    
    return *this;
}





SpQux::SpQux( SpQux&& qux )
    : SpFoo( std::forward<SpQux>(qux) )
{
    abbr = qux.abbr;
    cout << "SpQux move constructor\n";
}




SpQux& SpQux::operator = ( SpQux&& qux )
{
    dynamic_cast<SpFoo&>(*this) = std::forward<SpQux>(qux);
    abbr = qux.abbr;
    cout << "SpQux move assignment\n";
    
    return *this;
}





void SpQux::output()
{
    SpFoo::output();
    cout << "abbr = " << abbr << endl;
}

