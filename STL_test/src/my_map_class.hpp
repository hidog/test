#ifndef MY_MAP_CLASS_HPP
#define MY_MAP_CLASS_HPP

#include <iostream>

using namespace std;




template<int Prime>
class MyHash
{
public:
    // use for hash
    size_t operator () ( const int& key ) const
    {
        cout << "hash with key = " << key << ", mod = " << (key&hash_key) << endl;
        return key % hash_key;
    }
    
    // use for pred. 
    bool operator () ( const int& a, const int& b ) const
    {
        cout << "pred with a = " << a << " b = " << b << endl;
        return a % hash_key == b % hash_key;
    }
    
    int hash_key = Prime;
};







class GreaterThan
{
public:
    GreaterThan() = delete;
    GreaterThan(int v) : value(v) {}

    bool operator () (const pair<string,int>& p )
    {
        cout << "GreaterThan( pair ) " << p.first << ", " << p.second << endl;
        return p.second > value;
    }

private:
    int value;
};






class MyLess 
{
public:
    bool operator () ( const string& a, const string& b ) const
    {
        cout << "key a = " << a << ", key b = " << b << endl;
        int sum_a = 0;
        for( auto c : a )
            sum_a += c;
        
        int sum_b = 0;
        for( auto c : b )
            sum_b += c;
        
        return sum_a < sum_b;
    }
};


#endif



