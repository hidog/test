#include "initial_obj.h"

#include <iostream>

using namespace std;



InitialObj::InitialObj() 
    : ptr( new int[5]{1,2,3,4,5} )
{
    cout << "InitialObj()" << endl;
}





InitialObj::InitialObj( int _d1, int _d2 ) 
    : data1(_d1), data2(_d2)
{
    cout << "InitialObj(int,int)" << endl;
}




InitialObj::InitialObj( const InitialObj& aa ) 
    : InitialObj() 
{}






InitialObj::InitialObj( std::initializer_list<int> list )
{
    cout << "InitialObj(initializer_list<int>" << endl;
    
    // note: generally, check list.size() != 0.
    
    int count = 0;
    ptr = new int[ list.size() + 1 ];
    for( auto itr = list.begin(); itr != list.end(); ++itr )
        // for( int i = 0, auto itr = list.begin(); ... ) will compile error
        ptr[count++]  =   *itr ;
}






InitialObj::InitialObj( std::initializer_list<short> list )
{
    cout << "A(initializer_list<short>" << endl;

    int count = 0;
    ptr = new int[ list.size() + 1 ];
    for( auto itr = list.begin(); itr != list.end(); ++itr )
        // for( int i = 0, auto itr = list.begin(); ... ) will compile error
        ptr[count++]  =   *itr;
}






InitialObj::~InitialObj()
{
    delete [] ptr;
}

