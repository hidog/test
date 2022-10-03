#include "p33.h"

#include <vector>
#include <iostream>

using namespace std;


class Solution {
public:
    int search(vector<int>& nums, int target) {
        
        if( nums.size() == 1 && target == nums[0] )
            return 0;
        else if( nums.size() == 1 && target != nums[0] )
            return -1;
        
        if( nums.size() == 2 )
        {
            if( target == nums[0] ) return 0;
            else if( target == nums[1] ) return 1;
            else return -1;
        }
        
        int start = 0, end = nums.size()-1;
        int half;
        int pivat = -1;
        while(true)
        {
            half = (start+end)/2;
            if( half == 0 && nums[half] > nums[half+1] )
            {
                pivat = 1;
                break;
            }
            else if( nums[half] < nums[half-1] )
            {
                pivat = half;
                break;
            }
            else if( nums[half] > nums[0] )
                start = half+1;
            else
                end = half-1;
            
            if( start >= end )
            {
                if( nums[start] < nums[start-1] )
                    pivat = start;
                break;
            }
        }
        
        auto bs = [&](int s0, int e0) -> int {
            int s = s0, e = e0, h;
            while(true)
            {
                h = (s+e)/2;
                if( nums[h] == target )
                    return h;
                else if( nums[h] > target )                
                    e = h-1;
                else
                    s = h+1;       
                if( s > e )
                    return -1;
            }
        };
        
        if( pivat == -1 )
            return bs(0, nums.size()-1);
        else if( target >= nums[pivat] && target <= nums[nums.size()-1] )
            return bs(pivat, nums.size()-1);
        else if( target == nums[pivat] )
            return pivat;
        else
            return bs(0, pivat);
                
        return -1;
    }
};



void p33_Search_in_Rotated_Sorted_Array()
{
    Solution so;
    vector<int> v{5,1,2,3,4};
    int t = 4;
    cout << so.search(v, t);
}