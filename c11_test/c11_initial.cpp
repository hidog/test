#include "c11_initial.h"
#include <iostream>

using namespace std;



namespace {




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


} // end namespace





namespace c11 {


void uniform_initial_test()
{
    // initialized, and
    int i;
    
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





void initial_list_test()
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







} // end namespace c11