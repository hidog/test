#include "c11_range_for.h"
#include <vector>
#include <iostream>
#include "class/rule_five_class.h"

using namespace std;




namespace c11 {


void range_for_test_1()
{
    vector<int> a{ 1, 2, 3, 4, 5 };
    
    for( auto item : a )
        item += 10;
    
    for( auto item : a )
        cout << item << " ";
    cout << endl;
    
    //
    for( auto &item : a )
        item += 10;
    
    for( auto item : a )
        cout << item << " ";
    cout << endl;
}


void range_for_test_2()
{
    Array a(5);
    
    for( auto &item : a )
        item *= 10;
    
    for( auto item : a )
        cout << item << " ";
    cout << endl;
    
}


} // end namespace c11
