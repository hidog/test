#include "c11_using.h"

#include <stdio.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>


using namespace std;


// 將樣板的兩個型態引數變成單引數.
template<typename value>
using String_map = map<std::string,value>;


/* 
效果同typedef
也可以用來定義function pointer, 樣板等等.
*/
using test_int = int;
using VectorInt = vector<int>;
using FuncCB = double(int,double);  // function point






namespace c11 {



void using_example_1()
{
    test_int i = 10;
    printf( "%d\n", i );

    VectorInt vec = { 6, 5, 4, 3, 1, 6, 7, 8, 9 };
    for( auto item : vec )
        cout << item << " ";
    cout << endl;

    FuncCB* fb = [](int a, double b) -> double 
    {
        double result;
        result = 0.5*a + b;
        return result;
    };

    double r = fb( 10, 66.23 );
    cout << "r = " << r << endl;
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
