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






 /* Definition for singly-linked list.*/
 struct ListNode {
      int val;
      ListNode *next;
      ListNode() : val(0), next(nullptr) {}
      ListNode(int x) : val(x), next(nullptr) {}
      ListNode(int x, ListNode *next) : val(x), next(next) {}
 };
 

ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) 
{
    int carry = 0;
    int value = 0;
    ListNode *result = new ListNode;
    ListNode *res_head = result;
    
    while(true)
    {            
        value = result->val;
        if( l1 != nullptr )
            value += l1->val;
        if( l2 != nullptr )
            value += l2->val;
    
        if( value >= 10 )
            carry = 1;
        else
            carry = 0;
        value %= 10;

        result->val = value;           
        
        if( l1 != nullptr )
            l1 = l1->next;
        if( l2 != nullptr )
            l2 = l2->next;
        
        if( l1 == nullptr && l2 == nullptr )
        {
            if( carry > 0 )
                result->next = new ListNode(carry);
            break;
        }
        else            
            result->next = new ListNode(carry);
        result = result->next;
    }
    
    return res_head;    
}




void    p2_Add_Two_Numbers()
{
#if 1
    ListNode* l1 = new ListNode(9);
    ListNode* start_l1 = l1;
    l1->next = new ListNode(9); 
    l1 = l1->next;
    l1->next = new ListNode(9); 
    l1 = l1->next;
    l1->next = new ListNode(9); 
    l1 = l1->next;
    l1->next = new ListNode(9); 
    l1 = l1->next;
    l1->next = new ListNode(9);
    ListNode* l2 = new ListNode(9);
    ListNode* start_l2 = l2;
    l2->next = new ListNode(9); 
    l2 = l2->next;
    l2->next = new ListNode(9); 
#else
    ListNode* l1 = new ListNode(2);
    ListNode* start_l1 = l1;
    l1->next = new ListNode(4); 
    l1 = l1->next;
    l1->next = new ListNode(3); 

    ListNode* l2 = new ListNode(5);
    ListNode* start_l2 = l2;
    l2->next = new ListNode(6); 
    l2 = l2->next;
    l2->next = new ListNode(4); 
#endif
    ListNode* res = addTwoNumbers(start_l1,start_l2);
    while( res != nullptr )
    {
        cout << res->val;
        res = res->next;
    }
    cout << endl;
}





int lengthOfLongestSubstring(string s) 
{
    if( s.size() == 0 )
        return 0;
    
    int start = 0;
    int end = 0;
    int table[256] = {0};
    
    int max = 0;
    
    while(true)
    {
        if( table[ s[end] ] == 1 )        
        {
            if( start == end )
            {
                if( max < end - start + 1 )
                    max = end - start + 1;                
                break;
            }
            table[ s[start] ] = 0;
            start++;
        }
        else
        {            
            if( max < end - start + 1 )
                max = end - start + 1;
            table[ s[end] ] = 1;
            end++;
            if( end >= s.size() )
                break;
        }
    }
    
    return max;
}




void    p3_Longest_Substring_Without_Repeating_Characters()
{
    string s = "";
    cout << lengthOfLongestSubstring(s);
}






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






bool isPalindrome(int x) 
{        
    if( x < 0 )
        return false;
    
    int64_t y = x, z = 0;
    while( y > 0 )
    {
        z = z*10 + y%10;
        y /= 10;            
    }
    if( z == x )
        return true;
    else
        return false;
}

void    p9_PalindromeNumber()
{
    cout << isPalindrome(2147483647) ? "yes" : "no";
}

