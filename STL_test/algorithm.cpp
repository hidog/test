#include "algorithm.h"

#include <map>
#include <string>
#include <iostream>

using namespace std;


namespace {

    
class GreaterThan
{
public:
    GreaterThan() = delete;
    GreaterThan(int v) : value(v) {}

    bool operator () (const pair<string,int>& p )
    {
        return p.second > value;
    }

private:
    int value;
};


void find_if_func( map<string,int>& map )
{
    /* 
        可以使用function object. 
        GreaterThan是一個物件,但使用上可以像是一個函數
        GreaterThan greater_than(100);    
        pair<string,int> p = { "aaa", 100 };
        greater_than(p);
    */
    GreaterThan greater_than(555);    
    pair<string,int> p = { "aaa", 777 };
    bool flag = greater_than(p);
    if( flag == true )
        cout << "true!!\n";

    auto v = find_if( map.begin(), map.end(), GreaterThan{199} ); 
    cout << v->first << " " << v->second << endl;

    auto v2 = find_if( map.begin(), map.end(), greater_than ); 
    cout << v2->first << " " << v2->second << endl;
}

} // end namespace 




void find_if_test()
{
    map<string,int> m = { {"a", 10}, {"b", 555}, {"c", 666} };
    find_if_func(m);
}
