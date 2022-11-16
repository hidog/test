#include "p152.h"

#include <iostream>
#include <vector>

using namespace std;



class Solution {
public:
    int get_max_product( vector<int>& nums, int start, int end )
    {
        if( end < 0 )
            return nums[start];
        if( start == end )
            return nums[start];       
        
        int odd = 0;
        int first_ne = -1, last_ne = -1;
        for( int i = start; i <= end; i++ )
        {
            if( nums[i] < 0 )
            {
                odd++;
                if( first_ne == -1 )
                    first_ne = i;
                last_ne = i;
            }
        }
        
        if( odd % 2 == 0 )
        {
            int prod = 1;
            for( int i = start; i <= end; i++ )
                prod *= nums[i];
            return prod;
        }
        else if( first_ne == last_ne )       
        {
            int prod_left = 1, prod_right = 1;
            for( int i = start; i < first_ne; i++ )
                prod_left *= nums[i];
            for( int i = last_ne+1; i <= end; i++ )
                prod_right *= nums[i];            
            return prod_left > prod_right ? prod_left : prod_right;
        }        
        else
        {
            int prod_center = 1, prod_left = 1, prod_right = 1;
            for( int i = first_ne+1; i < last_ne; i++ )
                prod_center *= nums[i];
            for( int i = start; i < first_ne; i++ )
                prod_left *= nums[i];
            for( int i = last_ne+1; i <= end; i++ )
                prod_right *= nums[i];
            
            int max1 = prod_left * nums[first_ne] * prod_center;
            int max2 = prod_right * nums[last_ne] * prod_center;
            
            return max1 > max2 ? max1 : max2;
        }
    }
    
    int maxProduct(vector<int>& nums) {   
        if( nums.size() == 1 )
            return nums[0];
        
        int last = 0;
        int max = nums[0];
        int index;
        for( index = 0; index < nums.size(); index++ )
        {
            if( nums[index] == 0 )
            {
                if( max < 0 )
                    max = 0;
                int tmp = get_max_product( nums, last, index-1 );
                if( tmp > max )
                    max = tmp;
                last = index + 1;                
            }
        }
        
        if( last != 0 && last < nums.size() )
        {
            int tmp = get_max_product( nums, last, nums.size()-1 );
            if( tmp > max )
                max = tmp;
        }        
        else if( index == nums.size() && last == 0 )
        {
            int tmp = get_max_product( nums, 0, nums.size()-1 );
            if( tmp > max )
                max = tmp;
        }
        
        return max;
    }
};


void p152_Maximum_Product_Subarray()
{
    vector<int> v{-2,0};
    Solution so;
    cout << so.maxProduct(v) << endl;;
}