#include "p8.h"

#include <list>
#include <map>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unordered_map>



using namespace std;





class FreqStack 
{

public:

    FreqStack() 
    {}
    
    void push(int val) 
    {
        data.push_front(val);
        count_map[val]++;
    }
    
    int pop() 
    {        
        int max = 0;
        for( const auto &itr : count_map )
        {
            if( itr.second >= max )
                max = itr.second;
        }
        
        int res = 0;
        for( auto itr = data.begin(); itr != data.end(); ++itr )
        {
            int r = *itr;
            if( count_map[r] == max )
            {
                res = r;
                count_map[r]--;
                data.erase(itr);
                break;
            }
        }
        
        return res;
    }
    
    list<int> data;
    unordered_map<int,int> count_map;
};




void p895_Maximum_Frequency_Stack()
{
    srand(time(0));

    FreqStack* obj = new FreqStack();
    for( int i = 0; i < 80000; i++ )
    {
        int r1 = rand() % 2;
        if( r1 == 0 )
            cout << obj->pop() << endl;
        else
        {
            int r2 = rand() % 10;
            obj->push(r2);
        }
    }

}