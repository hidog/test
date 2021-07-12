#include "c11_using.h"

#include <stdio.h>
#include <iostream>
#include <map>
#include <string>


using namespace std;

template<typename value>
using String_map = map<std::string,value>;



namespace c11 {


using test_int = int;


void using_example()
{
    test_int i = 10;
    printf( "%d\n", i );
}


void using_example_2()
{
    String_map<int> a;
    a.insert( make_pair("a", 1) );
    a.insert( make_pair("b", 2) );
    a.insert( make_pair("c", 3) );

    String_map<double> b;
    b.insert( make_pair("d", 44.652) );
    b.insert( make_pair("e", -1.1) );
    b.insert( make_pair("f", 0.123) );

    for( const auto &itr : a )
       cout << itr.first << " " << itr.second << endl;

    for( auto itr = b.begin(); itr != b.end(); ++itr )
        cout << itr->first << " " << itr->second << endl;
}



} // end namespace c11
