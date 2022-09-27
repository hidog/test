#include "p4.h"

#include <iostream>
#include <vector>

using namespace std;



double findMedianSortedArrays(vector<int>& nums1, vector<int>& nums2) 
{
    int mn = nums1.size() + nums2.size();
    int count = 0;
    int n1 = 0, n2 = 0;
    vector<int> a;
    
    while(true)
    {
        if( n1 >= nums1.size() && n2 >= nums2.size() )
            break;
        else if( n2 >= nums2.size() )
        {
            a.push_back( nums1[n1] );
            n1++;
        }
        else if( n1 >= nums1.size() )
        {
            a.push_back( nums2[n2] );
            n2++;
        }
        else if( nums1[n1] < nums2[n2] )
        {
            a.push_back( nums1[n1] );
            n1++;
        }
        else
        {
            a.push_back( nums2[n2] );
            n2++;
        }
    }
    
    if( a.size() == 0 )
        return 0;
    else if( a.size() % 2 == 1 )
        return a[ a.size()/2 ];
    else
        return 0.5*(a[a.size()/2] + a[a.size()/2-1]);
}


void    p4_Median_of_Two_Sorted_Arrays()
{
    vector<int> a{3,4,5};
    vector<int> b{2,5,6};
    cout << findMedianSortedArrays( a, b );
}


