#include "p1.h"

#include <vector>
#include <iostream>
#include <cstring>

using namespace std;



vector<int> twoSum(vector<int>& nums, int target)
{
    vector<int> res(2);
    for( int i = 0; i < nums.size(); i++ )
    {
        for( int j = i+1; j < nums.size(); j++ )
        {
            if( nums[i] + nums[j] == target )
            {
                res[0] = i;
                res[1] = j;
                break;              
            }        
        }    
    }
    return res;
}



void    p1_two_sum()
{
    vector<int> nums { 2, 7, 11, 15 };
    vector<int> res = twoSum( nums, 9 );
    cout << res[0] << ", " << res[1] << endl;
}



