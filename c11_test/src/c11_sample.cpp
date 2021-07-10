#include "c11_sample.h"
#include <iostream>
#include <vector>
#include <list>
#include <deque>
#include <cstdlib>
#include <map>
#include <utility>
#include <memory>
#include <cstring>

using namespace std;






int right_value()
{
    static int r_value = 3;
    cout<< r_value << endl;
    return r_value;
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
