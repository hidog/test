#include "p169.h"

#include <iostream>
#include <vector>
#include <map>

using namespace std;



class Solution {
public:
    int majorityElement(vector<int>& nums) 
    {
        map<int,int> cm;
        int max = 0, max_value = 0;

        for( const auto& itr : nums )
        {
        	cm[itr]++;
        	if( max < cm[itr] )
        	{
        		max = cm[itr];
        		max_value = itr;

        	}
        }

        return max_value;
    }
};



void	p169_Majority_Element()
{
	Solution so;
	vector<int> v{ 2,2,1,1,1,2,2 };
	cout << so.majorityElement(v) << endl;

}