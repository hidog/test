#include "p53.h"

#include <vector>
#include <iostream>
#include <algorithm>
#include <tuple>
#include <cstdint>

using namespace std;



class Solution {
public:
    std::tuple<int,int> find_first_non_negative( vector<int>& nums )
    {
        int i;
        int max = INT32_MIN;
        for( i = 0; i < nums.size(); ++i )
        {
            if( nums[i] >= 0 )
                break;
            if( nums[i] > max )
                max = nums[i];
        }
        
        return std::tie<int,int>(i,max);
    }
    
    std::tuple<int,int> find_conti_negative( vector<int>& nums, int index )
    {
        int i;
        int count = 0;
        for( i = index; i < nums.size(); i++ )
        {
            if( nums[i] >= 0 )
                break;
            count += nums[i];
        }
        return std::tie<int,int>(i,count);
    }
    
    int maxSubArray(vector<int>& nums) {        
        auto non_ne = find_first_non_negative(nums);
        int first_ne = std::get<0>(non_ne);
        int max_ne = std::get<1>(non_ne);
        
        if( first_ne == nums.size() )
            return max_ne;
        
        int max = nums[first_ne];
        int sum = nums[first_ne];
        
        for( int i = first_ne+1; i < nums.size(); i++ )
        {
            if( nums[i] < 0 )
            {
                auto conti_ne = find_conti_negative( nums, i );
                int idx = std::get<0>(conti_ne);
                int nc = std::get<1>(conti_ne);
                
                if( idx >= nums.size() )
                    break;
                else if( sum + nc < 0 )
                {
                    if( max < sum )
                        max = sum;
                    i = idx;
                    sum = nums[idx];
                }
                else if( sum + nc >= 0 )
                {
                    if( max < sum )
                        max = sum;
                    sum += nc + nums[idx];
                    i = idx;
                }
            }
            else
            {
                sum += nums[i];
                if( max < sum )
                    max = sum;
            }
        }
        
        if( max < sum )
            max = sum;
        
        return max;        
    }
};



void p53_Maximum_Subarray()
{
    Solution so;
    vector<int> v {8,-19,5,-4,20};
    cout << so.maxSubArray(v) << endl;
}