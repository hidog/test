#include "c11_swap.h"

#include <iostream>
#include <cstring>



using namespace std;









class SWAP_TEST
{
public:
    SWAP_TEST() 
        : ptr(nullptr), size(0) 
    { 
        cout << "constructor" << endl; 
    }

    ~SWAP_TEST()
    {
        delete [] ptr;
        ptr = nullptr;
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

    SWAP_TEST( SWAP_TEST&& st ) noexcept
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


    SWAP_TEST& operator = ( SWAP_TEST&& st ) noexcept
    {

#if 1
        // need set size, ptr as 0, null.
        std::swap( size, st.size );
        std::swap( ptr, st.ptr );
#else
        // allowed &st. it will crash without this if sentences.
        if( this != &st ) 
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









namespace c11 {



void swap_test()
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



} // end namespace c11