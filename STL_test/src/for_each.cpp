#include "for_each.h"
#include <deque>
#include <iostream>
#include <algorithm>
#include <vector>
#include <list>



using namespace std;



namespace {


// initializer list
template<typename T>
std::deque<T> sort_alpha( std::initializer_list<T> list )
{
    std::deque<T>    deq{list};

    if( list.size() > 0 )
    {
        for( size_t i = 0; i < deq.size(); i++ )
            for( size_t j = 0; j < deq.size()-1; j++ )
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






} // end namespace







void for_each_example()
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
    for( size_t i = 0; i < v.size(); i++ )
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

