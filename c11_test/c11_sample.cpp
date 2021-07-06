#include "c11_sample.h"
#include <iostream>
#include <vector>
#include <list>
#include <deque>
#include <cstdlib>
#include <map>
#include <utility>
#include <algorithm>
#include <memory>
#include <cstring>

using namespace std;


// initialized, and
void c11_sample_1()
{
    int     i;
    
    // Uniform initialization
    int a1 = {-5};
    cout << "a1 = " << a1 << endl;
    
    //
    float a2{5.44};
    cout << "a2 = " << a2 << endl;
    
    //
    unsigned int a3(5);  // c99
    cout << "a3 = " << a3 << endl;
    
    //
    double a4[5]{1.1, 2.2, 3.3, 4.4};
    for( i = 0; i < 5; i++ )
        cout << a4[i] << " ";
    cout << endl;
    
    //
    char a5[5] = { 'a', 'b', 'c', 'd', 'e' };   // c99
    for( i = 0; i < 5; i++ )
        cout << a5[i] << " ";
    cout << endl;
    
    //
    int *ptr = new int[3]{ 5, 9, 3 };
    for( i = 0; i < 3; i++ )
        cout << ptr[i] << " ";
    cout << endl;
    
    //
    //int a6{2.34};   // compile error. can't convert double to int.
    int a7 = 2.34;    // ok. a7 = 2   convert double to int
    cout << a7 << endl;
}



// initializer list
template<typename T>
std::deque<T> sort_alpha( std::initializer_list<T> list )
{
    std::deque<T>    deq{list};
    
    if( list.size() > 0 )
    {
        for( int i = 0; i < deq.size(); i++ )
            for( int j = 0; j < deq.size()-1; j++ )
            {
                if( deq[j] > deq[j+1] )
                    deq[j] ^= deq[j+1] ^= deq[j] ^= deq[j+1];
            }
    }
    else
    {
        deq.clear();
        deq.push_back( T() );
        deq.push_back( T{} );
        deq.push_back( T()+1 ); // 並不是一個好的寫法,僅為測試用
    }
    
    return  deq;
}




template<typename T>
void print_deque( T a )
{
    cout << "print " << a << " ";
}



//
void c11_sample_2()
{
    //
    std::deque<char> deq = sort_alpha<char>( {'a', 'z', 'd', 'e', 'h', 'i', 'x', 'y', 'b', 'u', 'v'} );
    std::for_each( deq.begin(), deq.end(), []( char& c )    // it can use char& or char
                  {
                      cout << c << " ";
                  });
    cout << endl;
    
    //
    std::deque<int> deq2 = sort_alpha<int>( {} );
    std::for_each( deq2.begin(), deq2.end(), print_deque<int> );
    cout << endl;
    
    //
    std::vector<double> v = { 1.1, 2, 3, 4.4 };
    for( int i = 0; i < v.size(); i++ )
        cout << v[i] << " ";
    cout << endl;
    
    //
    std::list<int> list{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    for( auto itr = list.begin(); itr != list.end(); ++itr )
        cout << *itr << " ";
    cout << endl;
    
    // note: for迴圈有更多應用方式,這邊不多作介紹.
    // search Range-based for loop, see http://en.cppreference.com/w/cpp/language/range-for
    for ( int x : {-1, -2, -3} )
        cout << x << " ";
    cout << endl;
    
    //
    auto ptr = new int[3]{100, 99, 98};
    cout << ptr[0] << " " << ptr[1] << " " << ptr[2] << endl;
    delete [] ptr;
    
    //
    std::vector<int> *v_ptr = new std::vector<int>{-1,-2,-3,4,5};
    for( int i = 0; i < 5; i++ )
        cout << (*v_ptr)[i] << " ";
    cout << endl;
    
    auto aaa = 1000;
    cout << aaa << endl;
}



class B
{
public:
    B( int _a ) : a(_a)
    {}
    
    B( int _a, double _d ) : a(_a), d(_d)
    {}
    
    int a = 4;
    //int b(3);  // compile fail.
    int c{4};
    double d;
};



class A
{
public:
    A() : ptr( new int[5]{1,2,3,4,5} )
    {
        cout << "A()" << endl;
    }
    
    A( int _d1, int _d2 ) : data1(_d1), data2(_d2)
    {
        cout << "A(int,int)" << endl;
    }
    
    A( const A& aa ) : A() {}
    
    // initializer list
#if 1
    A( std::initializer_list<int> list )
    {
        cout << "A(initializer_list<int>" << endl;
        
        // note: generally, check list.size() != 0.
        
        int count = 0;
        ptr = new int[ list.size() ];
        for( auto itr = list.begin(); itr != list.end(); ++itr )
        // for( int i = 0, auto itr = list.begin(); ... ) will compile error
            ptr[count++]  =   *itr;
    }

    // it will run initializer_list<int>, not run initializer_list<short>
    A( std::initializer_list<short> list )
    {
        cout << "A(initializer_list<short>" << endl;
        
        int count = 0;
        ptr = new int[ list.size() ];
        for( auto itr = list.begin(); itr != list.end(); ++itr )
            // for( int i = 0, auto itr = list.begin(); ... ) will compile error
            ptr[count++]  =   *itr;
    }
#endif

    
    ~A()
    {
        delete [] ptr;
    }
    
    int data1 = 11;
    int data2{99};
    int *ptr = nullptr;
    
    //B b(99); // compile fail.
    B b1{99};
    B b2 = {99};
    B b3 = {-4, 4.5442};
    
};







void c11_sample_3()
{
    int i;
    
    A a1;
    for( i = 0; i < 5; i++ )
        cout << a1.ptr[i] << " ";
    cout << a1.data1 << " " << a1.data2 << endl;
    
    A a2(3,4);   // c99
    cout << a2.data1 << " " << a2.data2 << " " << a2.b3.d << endl;
    
    // if use initialer_list in constructor, a3 and a4 will run A(initializer_list), not A(int,int)
    A a3{ 2, 3 };
    cout << a3.data1 << " " << a3.data2 << endl;
    
    A a4 = { 36, -53 };
    cout << a4.data1 << " " << a4.data2 << endl;
    
    A a5( {-1,2,0,4} );
    //A a5{ {-1,2,0,4} }; 可以使用大括號
    for( i = 0; i < 4; i++ )
        cout << a5.ptr[i] << " ";
    cout << endl;

    //int aaa = nullptr; // compile fail.
    int bbb = NULL; // this is difference between nullptr and NULL.
    cout << bbb << endl;
    
}



int& left_value()
{
    static int l_value = 0;
    return l_value;
}

int right_value()
{
    static int r_value = 3;
    cout<< r_value << endl;
    return r_value;
}

void ref_test_1( int &x )
{
    cout << ++x << endl;
}

void ref_test_2( const int&x )
{
    cout << x << endl;
}

void ref_test_3( int&& x)
{
    cout << ++x << endl;
}

int ref_test_4()
{
    return  -4723;
}

// left value, right value
// http://eli.thegreenplace.net/2011/12/15/understanding-lvalues-and-rvalues-in-c-and-c
void c11_sample_4()
{
    // left value
    int i = 42;
    int *p = &i; // i is left value
    cout << *p << endl;
    
    int *p2 = &left_value(); // left_value() is left value.
    *p2 = 55;
    cout << left_value() << endl; // l_value is 55 now.
    
    int *p3 = new int[3]{1,2,3};
    cout << *(p3 + 1) << endl; // p3+1 is right value, *(p3+1) is left value.
    //int **pp = &(p3+1); // compile fail. because p3+1 is right value.
    
    // right value
    //int *p3 = &right_value(); // compile fail. it is right value.
    int j = 925;  // 925 is right value.  (tmp value)
    int a = 3, b = 4; // a, b is left value.
    int c = a + b; // a+b is right value.
    cout << j << " " << c << endl;
    //int *q = &(j+1); // compile error. j+1 is right value.
    
    //
    std::string str = "abc";
    std::string& str2 = str; // left value reference
    //std::string& str3 = "abc";  // compile fail. right value can't be referenced.
    const std::string& str3 = "abc"; // success. right value can be reference by const .
    cout << str2 << " " << str3 << endl;
    
    
    
    // reference
    int r = 10;
    //int &r1 = 3; // compile fail.
    int &r2 = r;
    //int &&r3 = r; // compile fail.
    int &&r4 = 3;
    
    cout << r2 << " " << r4 << endl;
    r2++; r4++;
    cout << r << " " << r4 << endl;
    
    int rr1 = 3, rr2 = 4;
    //int &rr3 = rr1 + rr2; // compile fail.
    int &&rr4 = rr1 + rr2;
    cout << rr4 << endl;
    rr4 += 1000;
    cout << rr4 << " " << rr1 << " " << rr2 << " " << endl;
    
    
    // function parameter
    ref_test_1( r );
    //ref_test_1( 3 ); // compile fail
    
    ref_test_2( r );
    ref_test_2( 3 );
    
    //ref_test_3( r ); // compile fail.
    ref_test_3( 3 );
    
    cout << r << endl;
    
    int rrr1 = ref_test_4();
    //int &rrr2 = ref_test_4(); // compile fail.
    const int &rrr3 = ref_test_4();
    int &&rrr4 = ref_test_4();
    
    cout << rrr1 << rrr3 << rrr4 << endl;
}




class C1
{
public:
    C1() : ptr(NULL), size(0)
    {
        cout << "C1(), addr = " << this << endl;
    }
    
    C1( int _s )
    {
        cout << "C1(int), addr = " << this << endl;
        size = _s;
        ptr = new int[size];
        for( int i = 0; i < size; i++ )
            ptr[i]  =   rand() % 10000;
    }
    
    C1( const C1& _c )
        : C1() // c++11 only
    {
        cout << "C1(const C1&), addr = " << this << endl;
        size = _c.size;
        ptr = new int[size];
        for( int i = 0; i < size; i++ )
            ptr[i]  =   _c.ptr[i];
    }
    
    ~C1()
    {
        cout << "~C1(), addr = " << this << endl;
        delete [] ptr;
    }
    
    C1& operator = ( const C1& _c )
    {
        cout << "C1 operator = , addr = " << this << " , _c addr = " << &_c << endl;
        
        if( size != 0 && ptr != NULL )
            delete [] ptr;
        
        size = _c.size;
        ptr = new int[size];
        for( int i = 0; i < size; i++ )
            ptr[i] = _c.ptr[i];
        
        return *this;
    }
    
    int size;
    int *ptr;
};


C1 C1_test_1( C1 c )
{
    cout << "c addr = " << &c << endl;
    return C1( c.size + 10 );
    
    /*
     note: Return value optimization (RVO),
     result is difference from 
     C1 tmp;
     ...
     return tmp;
     */
}


// traditional rule of three.
// 姑且不論寫法上有一些瑕疵
void c11_sample_5()
{
    C1 c1(10000);
    cout << c1.ptr[0] << endl;
    cout << "c1 addr = " << &c1 << endl;
    
    C1 c2;
    cout << "c2 addr = " << &c2 << endl;
    c2 = C1_test_1( c1 );
    cout << c2.ptr[0] << endl;
}









void c11_sample_6()
{
    string str = "hello";
    vector<string> vec;
    
    vec.push_back(str);
    cout << vec[0] << " " << str << endl;
    
    vec.push_back( move(str) );
    cout << vec[1] << " " << str << endl;
}








class C2
{
public:
    C2() : ptr(NULL), size(0)
    {
        //cout << "C2(), addr = " << this << endl;
        cout << "constructor C2()" << endl;
    }
    
    C2( int _s )
    {
        cout << "constructor C2(int)" << endl;
        //cout << "C2(int), addr = " << this << endl;
        size = _s;
        ptr = new int[size];
        for( int i = 0; i < size; i++ )
            ptr[i]  =   rand() % 10000;
    }
    
    C2( const C2& _c )
    {
        cout << "copy constructor" << endl;
        //cout << "C2(const C2&), addr = " << this << endl;
        size = _c.size;
        ptr = new int[size];
        for( int i = 0; i < size; i++ )
            ptr[i]  =   _c.ptr[i];
    }
    
    C2( C2&& _c )
    {
        cout << "move constructor\n";
        
        ptr     =   _c.ptr;
        size    =   _c.size;
        
        _c.ptr  =   nullptr;
        _c.size =   0;
    }
    
    
    ~C2()
    {
        //cout << "~C2(), addr = " << this << endl;
        delete [] ptr;
    }
    
    C2& operator = ( const C2& _c )
    //C2& operator = ( C2& _c )
    {
        cout << "copy operator = , addr = " << this << " , _c addr = " << &_c << endl;
        
        if( size != 0 && ptr != NULL )
            delete [] ptr;
        
        size = _c.size;
        ptr = new int[size];
        for( int i = 0; i < size; i++ )
            ptr[i] = _c.ptr[i];
        
        return *this;
    }
    
    
    // move assignment
    C2& operator = ( C2&& _c )
    {
        cout << "C2 move operator = , addr = " << this << endl;
        
        ptr = _c.ptr;
        size = _c.size;
        
        _c.ptr = nullptr;
        _c.size = 0;
        
        return *this;
    }
    
    
    int size;
    int *ptr;
};




C2 C2_test_1( C2 c )
{
    //cout << "c addr = " << &c << endl;
    return C2( c.size + 10 );
}


void c11_sample_7()
{
    //C2 &c2 = C2(1000);
    C2 c2a = C2(1000);
    C2 c2b = std::move( C2(2000) );
    cout << c2b.ptr[0] << endl;
    //cout << "c2a addr = " << &c2a << endl;
    
    C2 c2c = C2_test_1( c2a ); // run constructor
    C2 c2d = std::move( C2_test_1(c2b) );
    //cout << "c2b addr = " << &c2b << endl;
    c2c = C2_test_1( c2a );
    cout << c2c.ptr[0] << endl;
    
    c2c = c2a; // call copy assignment =
    cout << c2c.ptr[0] << endl;
    
    c2c = C2(321);  // call move assignment.
    cout << c2c.size << endl;
    
    /*
     if we don't define move assign, it will call assign.
     if we define assign as C2& operator = ( C2& _c ) without const, it will compile error.
     */
}








class D1
{
public:
    D1() : ptr(NULL), size(0) { cout << "constructor" << endl; }
    
    D1( int _s )
    {
        //cout << "D1(int), addr = " << this << endl;
        // note: need check _s > 0
        cout << "constructor" << endl;
        size = _s;
        ptr = new int[size];
        for( int i = 0; i < size; i++ )
            ptr[i]  =   rand() % 10000;
    }
    
    D1( const D1& _d )
    {
        cout << "copy constructor D1(const D1&), addr = " << this << endl;
        size = _d.size;
        ptr = new int[size];
        for( int i = 0; i < size; i++ )
            ptr[i]  =   _d.ptr[i];
    }
    
    // move constructor
    D1( D1&& _d )
    {
        cout << "move constructor D1(D1&&), addr = " << this << endl;
        size = _d.size;
        ptr = _d.ptr;
        
        _d.ptr = nullptr;
        _d.size = 0;
    }
    
    ~D1()
    {
        //cout << "~D1(), addr = " << this << endl;
        delete [] ptr;
    }
    
    
    D1 operator + ( const D1& _d )
    {
        D1 tmp( _d.size + size );
        //return D1( _d.size + size );
        return tmp;
    }

    
    int size;
    int *ptr;
};



D1 move_ct_test_1( D1 d )
{
    if( d.size > 0 )
        d.ptr[0]   =   1234;
    else
    {
        d.size = 1;
        d.ptr = new int[d.size];
    }
    
    return  d;
}


D1 move_ct_test_2( D1& d )
{
#if 1
    if( d.size > 0 )
        d.ptr[0]   =   9876;
    else
    {
        d.size = 1;
        d.ptr = new int[d.size];
    }
    
    return  d; // run copy constructor in D1 dd4 = move_ct_test_2(d2);
#else
    D1 tmp;
    tmp.size = d.size;
    tmp.ptr = new int[tmp.size];
    for( int i = 0; i < tmp.size; i++ )
        tmp.ptr[i] = d.ptr[i];
    
    return tmp; // run constructor in D1 dd4 = move_ct_test_2(d2);
#endif
}



D1&& move_ct_test_3( D1&& d )
{
    if( d.size > 0 )
        d.ptr[0]   =   -1234;
    else
    {
        d.size = 1;
        d.ptr = new int[d.size];
    }
    
    //return  d;  // compile fail.
    return move(d); // compile success.
    //return std::forward<D1>(d); //compile success.
    //return std::forward(d); // compile fail
}

D1 move_ct_test_4( int n )
{
    return D1(n + 10);
}


// note: 繼承關係參考 http://en.cppreference.com/w/cpp/language/move_assignment
void c11_sample_8()
{
    D1 d1(1000), d2(1500);

    //D1 d3( d1 + d2 );
    D1 d3( std::move(d1+d2) );
    cout << d1.ptr[0] << " " << d2.ptr[0] << " " << d3.ptr[0] << endl;

    /*
        this code will not run into move constructor.
        I gauss the reason is RVO.
     */
    D1 d4( move(d1+d2) );
    cout << d4.ptr[0] << endl;
    
    D1 dd1 = std::move(d1);
    cout << d1.size << " " << dd1.size << endl;

    D1 dd2 = move_ct_test_1(d2);
    cout << dd2.ptr[0] << endl;
    
    D1 dd3 = move_ct_test_1( D1(234) );
    cout << dd3.size << endl;
    
    D1 dd4 = move_ct_test_2(d2); // see comment move_ct_test_2
    cout << dd4.ptr[0] << endl;
    
    //D1 dd5 = move_ct_test_2( D(111) ); // compile fail.

    //D1 dd6 = move_ct_test_3( d2 ); // compile fail.
    D1 dd7 = move_ct_test_3( D1(323) );
    cout << dd7.ptr[0] << endl;
    
    D1 dd8 = move_ct_test_4( 1000 );
    cout << dd8.ptr[0] << endl;
}





class E
{
public:
    E() : ptr(NULL), size(0)
    {
        cout << "constructor" << endl;
    }
    
    E( int _s )
    {
        static int count = 1;
        cout << "constructor" << endl;
        size = _s;
        ptr = new int[size];
        for( int i = 0; i < size; i++ )
            ptr[i]  =   count++;
    }
    
    E( const E& _d )
    {
        cout << "copy constructor"  << endl;
        size = _d.size;
        ptr = new int[size];
        for( int i = 0; i < size; i++ )
            ptr[i]  =   _d.ptr[i];
    }
    
    // move constructor
    E( E&& e )
    {
        cout << "move constructor" << endl;
        size = e.size;
        ptr = e.ptr;
        
        e.ptr = nullptr;
        e.size = 0;
    }
    
    ~E()
    {
        cout << "destructor" << endl;
        delete [] ptr;
    }
    
    E& operator = (const E& e)
    {
        cout << "assign" << endl;
        
        if( size > 0 )
            delete [] ptr;
        
        size    =   e.size;
        ptr     =   new int[size];
        for( int i = 0; i < size; i++ )
            ptr[i]  =   e.ptr[i];
        
        return *this;
    }
    
    E& operator = ( E&& e)
    {
        cout << "move assign" << endl;
     
        if( size > 0 )
            delete [] ptr;
        
        size = e.size;
        ptr = e.ptr;
        
        e.size = 0;
        e.ptr = nullptr;
        
        return *this;
    }

    
    int size;
    int *ptr;
    
};


/*E operator + ( const E& e1, const E& e2 )
{
    cout << "+" << endl;
    E tmp;
    tmp.size = e1.size < e2.size ? e1.size : e2.size;
    tmp.ptr = new int[tmp.size];
    
    for( int i = 0; i < tmp.size; i++ )
        tmp.ptr[i] = e1.ptr[i] + e2.ptr[i];
    
    return tmp;
}*/


/*
    it will cause e1's real memory size is not equal e1.size.
 */
E&& operator + ( E e1, const E& e2 )
{
    cout << "+" << endl;

    e1.size = e1.size < e2.size ? e1.size : e2.size;
    
    for( int i = 0; i < e1.size; i++ )
        e1.ptr[i] += e2.ptr[i];
    
    return std::move(e1);
}



void c11_sample_9()
{
    // case 1
    E a(1000), b(1004), c(1003), d(1001), e(1002);
    cout << a.ptr[0] + b.ptr[0] + c.ptr[0] + d.ptr[0] + e.ptr[0] << endl;
    
    E sum = a + b + c + d + e;
    cout << sum.ptr[0] << endl; // call constructor 9 times.
    
    /*
     case 1:
        constructor 9
        destructor 9
     
     case 2:
        constructor = 5
        copy constructor = 1
        move constructor = 4
        destructor = 10  more by one copy constructor
     */
}




class F
{
public:
    F() : v{1,2,3,4,5,6,7,8,9}
    {}
    
    F( int n )
    {
        v.resize(n);
        for( int i = 0; i < n; i++ )
            v[i] = rand() * 100;
    }
    
    F( const F& f )
    {
        v   =   f.v;
        v[0]++;
    }
    
    F( F&& f )
    {
        v   =   move(f.v);
        v[0] = -v[0];
    }
    
    ~F(){}
    
    vector<int> v;
};



void c11_sample_10()
{
    F f1;
    cout << f1.v[0] << endl;
    
    F f2(f1);
    cout << f2.v[0] << endl;
    
    F f3(move(f1));
    cout << f3.v[0] << " " << f1.v.size() << endl;
    
    F f4 = move(F{100});    // note: if we don't use std::move, it will not call move constructor. (RVO)
    cout << f4.v[0] << " " << f4.v.size() << endl;
}




void pf_test_f1( int&& a )
{
    cout << a << endl;
    a += 111;
    cout << a << endl;
}



template<typename T>
void pf_test_f2( T&& a )
{
    cout << a << endl;
    a += 111;
    cout << a << endl;
}



void c11_sample_11()
{
    pf_test_f1(10);
    
    int a = 55;
    //pf_test_f1(a); // build fail.
    pf_test_f2(20);
    pf_test_f2( int{30} );
    pf_test_f2(a);
    cout << a << endl;
    
    
}








class G
{
public:
    G() : data(0) {}
    G(int d) : data(d) {}
    G(const G& g) : data(g.data) { cout << "copy constructor" << endl; }
    G(G&& g) : data(g.data) { cout << "move constructor" << endl; }
    
    int data;
};




struct AA
{
    AA() : aa(0) { cout << "constructor." << endl; }
    AA( int&& n ) : aa(n) { std::cout << "rvalue overload, n=" << n << endl; }
    AA( const int& n ) : aa(n) { std::cout << "lvalue overload, n=" << n << endl; }
    int aa;
};

class BB
{
public:
    template<class T1, class T2, class T3>
    BB(T1&& t1, T2&& t2, T3&& t3) :
        a1_{std::forward<T1>(t1)},
        a2_{std::forward<T2>(t2)},
        a3_{std::forward<T3>(t3)}
    {}
    
    // it will cause run lvalue overload.
    /*template<class T1, class T2, class T3>
    BB(T1&& t1, T2&& t2, T3&& t3) :
        a1_{t1},
        a2_{t2},
        a3_{t3}
    {}*/
    
    
private:
    AA a1_, a2_, a3_;
};


// U derive by u.
template<class T, class U>
std::unique_ptr<T> pf_test_1( U&& u )
{
    return std::unique_ptr<T>( new T(u) );
}



// U derive by u.
template<class T, class U>
std::unique_ptr<T> pf_test_2( U&& u )
//std::unique_ptr<T> pf_test_2( U u ) // cause both rvalue
{
    return std::unique_ptr<T>( new T( std::forward<U>(u) ) );
    //return std::unique_ptr<T>( new T(u) );  // cause all run lvalue.   ( u = 3; T(u); u is lvalue. )
}







template<class T, class... U>
std::unique_ptr<T> pf_test_3( U&&... u )
{
    return std::unique_ptr<T>( new T(std::forward<U>(u)...) );
}


std::unique_ptr<AA> pf_test_4( int&& u )
{
    return std::unique_ptr<AA>( new AA( std::forward<int>(u) ) );
}




std::unique_ptr<AA> pf_test_5( const int& u )
{
    return std::unique_ptr<AA>( new AA( std::forward<const int&>(u) ) );
}


void c11_sample_12()
{
    auto p = pf_test_1<AA>(10);
    
    // official sample
    auto p1 = pf_test_2<AA>(2); // rvalue
    //auto p1 = make_unique_1(2); // compile error
    //unique_ptr<AA> p1 = make_unique_1(3); // compile error
    
    int i = 1;
    auto p2 = pf_test_2<AA>(i); // lvalue
    
    std::cout << "B\n";
    auto t = pf_test_3<BB>(2, i, 3);
    
    auto p3 = pf_test_4(3);
    //auto p4 = pf_test_4(i); // compile fail.
    auto p5 = pf_test_5(i);
}





class PF1
{
public:
    PF1(){ cout << "PF1 constructor" << endl; }
    PF1( const PF1& pf ){ cout << "PF1 copy constructor" << endl; }
    PF1( PF1&& pf ){ cout << "PF1 move constructor" << endl; }
};



class PF2
{
public:
    PF2(){ cout << "PF2 constructor" << endl; }
    PF2( const PF2& pf ){ cout << "PF2 copy constructor" << endl; }
    PF2( PF2&& pf ){ cout << "PF2 move constructor" << endl; }
};




class PF
{
public:
    
    // you don't need define four constructor.
    // I have try for make_pair but fail.
    template<typename T, typename U>
    PF( T&& t, U&& u )
    : p1(std::forward<T>(t)), p2(std::forward<U>(u))
    {}

    PF1    p1;
    PF2    p2;
};





void c11_sample_13()
{
    PF  pf = { PF1(), PF2() };
    cout << endl;
    
    PF1 pf1;
    PF2 pf2;
    PF pf_a{ pf1, pf2 };
    cout << endl;
    
    PF pf_b( PF1(), pf2 );
}





class SWAP_TEST
{
public:
    SWAP_TEST() : ptr(nullptr), size(0) { cout << "constructor" << endl; }
    
    ~SWAP_TEST()
    {
        delete [] ptr;
        ptr = NULL;
        cout << "destsructor" << endl;
    }
    
    SWAP_TEST( int s )
    {
        size = s;
        ptr = new int[size];
        ptr[0] = 332;
        cout << "constructor s = " << s << endl;
        
        //int *ptr2 = new int[s];
        //ptr2[0] = 4252;
    }
    
    SWAP_TEST( const SWAP_TEST& st )
    {
        size = st.size;
        ptr = new int[size];
        memcpy( ptr, st.ptr, sizeof(int) * size );
        
        cout << "copy constructor" << endl;
    }
    
    SWAP_TEST( SWAP_TEST&& st )
        : size(0), ptr(nullptr)
    {
        // need set size, ptr as 0, null.
        std::swap( size, st.size );
        std::swap( ptr, st.ptr );
        
        cout << "move constructor " << endl;
        cout << size << " " << st.size << endl;
        cout << ptr << " " << st.ptr << endl;
    }
    
    

    /*SWAP_TEST& operator = ( const SWAP_TEST& st )
    {
#if 0
        cout << ptr << " " << st.ptr << endl;
        
        delete [] ptr;
        ptr = NULL;
        
        cout << st.ptr[0];  // crash if called by a = a
        
        size = st.size;
        ptr = new int[size];
        memcpy( ptr, st.ptr, sizeof(int) * size );
#else
        if( this != &st )
        {
            cout << ptr << " " << st.ptr << endl;
            
            delete [] ptr;
            ptr = NULL;
            
            cout << st.ptr[0];  // crash if called by a = a
            
            size = st.size;
            ptr = new int[size];
            memcpy( ptr, st.ptr, sizeof(int) * size );
        }
#endif
        return  *this;
    }*/
    
    
    SWAP_TEST& operator = ( SWAP_TEST&& st )
    {
        
#if 0
        // need set size, ptr as 0, null.
        std::swap( size, st.size );
        std::swap( ptr, st.ptr );
#else
        if( this != &st ) // allowed &st. it will crash without this if sentences.
        {
            ptr = st.ptr;
            size = st.size;
        
            st.size = 0;
            st.ptr = nullptr;
        }
#endif
        
        cout << ptr[0] << endl;
        
        return *this;
    }
    
    /*template<typename T>
    void operator = ( T&& st ) // note: SWAP_TEST& operator = ( SWAP_TEST& st ), not const.
    {
        std::swap( size, st.size );
        std::swap( ptr, st.ptr );
        
        cout << ptr[0] << endl;
        
        //return *this;
    }*/
    
    
    
    int size;
    int *ptr;
};



void c11_swap()
{

    SWAP_TEST a = std::move( SWAP_TEST(1000000) );
    cout << a.size << endl;


    a = std::move(a);
    cout << a.ptr << endl;
    
    SWAP_TEST b = SWAP_TEST(30);
    b.ptr[0] = 99999;
    
    //a = b;
    //cout << "a.ptr = " << a.ptr[0] << endl;
    
    
    a = SWAP_TEST(100000);
    cout << a.ptr[0] << endl;


}
